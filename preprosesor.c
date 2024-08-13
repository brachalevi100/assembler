#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "function.h"
#include "preprosesor.h"
#include "vector.h"
#define tree() calloc(1, sizeof(Tree))
#define mcro() calloc(1, sizeof(Mcro))

/**
 * @brief Enumeration of different row types encountered during preprocessing.
 */
static enum row_type {
	empty_line,
	docu_line,
	regular_line,
	mcro_defin,
	legal_mcro_defin,
	unlegal_mcro_defin,
	unlegal_mcro_call,
	endmcro_defin,
	legal_endmcro_defin,
	unlegal_endmcro_defin,

};
static enum flug {
	to_file,
	to_mcro,
	error_file
};

typedef struct mcro {
	char name[MAX_LABLE_LEN];
	char** content;
}Mcro;

/**
 * Checks the legality of a macro name and ensures it adheres to naming rules. It also verifies
 * that the provided macro name does not conflict with existing instruction or directive names.
 * Provides an error message if the name is invalid.
 *
 * @param word The potential macro name.
 * @param order_tree A pointer to the tree containing instruction, directive, and macro names.
 * @param error A pointer to store an error message if the name is invalid.
 * @return SUCCESS if the name is valid, ERROR if the name is invalid.
 */
static int legal_name(char* word, Tree* order_tree, char** error) {
	char c;
	int type = str_exist_type(word, order_tree);
	if (type != ERROR) {
		if (type == instruction) {
			sprintf((*error), "illegal mcro name, '%s' is alredy instruction name.", word);
		}
		else if (type == directive) {
			sprintf((*error), "illegal lable mcro, '%s' is alredy directive name.", word);
		}
		else if (type == mcro) {
			sprintf((*error), "illegal lable name, '%s' is alredy define as mcro name.", word);
		}
		return ERROR;
	}
	c = *word;
	if (c < 'A' || c > 'Z' && c < 'a' || c > 'z') {
		sprintf((*error), "illegal mcro name, mcro name have to begin whith letters.");
		return ERROR;
	}
	if (is_letters_and_numbers(word) == ERROR) {
		sprintf((*error), "illegal mcro name, mcro name must contain only letters and numbers.");
		return ERROR;
	}
	return SUCCESS;
}

/**
 * Handles the definition of a macro. Parses the provided macro definition line, validates the name,
 * and initializes a Mcro structure to store the macro content.
 *
 * @param count_line The current line number being processed.
 * @param as_name The name of the source assembly file.
 * @param line The line containing the macro definition.
 * @param order_tree A pointer to the tree containing instruction, directive, and macro names.
 * @param mcro_p A pointer to a pointer where the Mcro structure will be stored.
 * @return legal_mcro_defin if the macro definition is valid, unlegal_mcro_defin if the definition is invalid.
 */
static int mcro_definition(int count_line, char* as_name, char* line, Tree* order_tree, Mcro** mcro_p) {
	char error[MAX_ERROR_LEN], mcro_name[MAX_LABLE_LEN];
	int count_word;
	line = skip_word(line);/*skip mcro*/
	count_word = sscanf(line, "%s", mcro_name);
	if (count_word == 0) {
		sprintf(error, "mising mcro name.");
		print_error_or_worning(count_line, E, as_name, error);
		return unlegal_mcro_defin;
	}
	if (strlen(mcro_name) == MAX_LABLE_LEN) {/*MAX_LABLE_LEN is alredy 1 more then the legell length(because we add 1 for'\0'*/
		sprintf(error, "the length of the label is over the limit.");
		print_error_or_worning(count_line, E, as_name, error);
		return unlegal_mcro_defin;
	}
	line = skip_word(line);
	line = skip_spaces(line);
	if (line == NULL || *line == '\0') {
		if (legal_name(mcro_name, order_tree, &error) == ERROR) {
			print_error_or_worning(count_line, E, as_name, error);
			return unlegal_mcro_defin;
		}
		/*legal defntion*/
		*mcro_p = (Mcro*)calloc(1, sizeof(Mcro));
		if (*mcro_p == NULL) {
			print_error_or_worning(ERROR_MEMORY_ALLOC, E, "C", "memory allocation failure.");
			return REAL_ERROR;
		}
		strcpy((*mcro_p)->name, mcro_name);
		(*mcro_p)->content = (char**)calloc(1, sizeof(char*));
		if ((*mcro_p)->name == NULL || (*mcro_p)->content == NULL) {
			print_error_or_worning(ERROR_MEMORY_ALLOC, E, "C", "memory allocation failure.");
			free(*mcro_p);
			return REAL_ERROR;
		}
		insert_str((*mcro_p)->name, order_tree, *mcro_p, mcro);
		return legal_mcro_defin;
	}
	else {
		sprintf(error, "extra parameters after mcro defintion.");
		print_error_or_worning(count_line, E, as_name, error);
		return unlegal_mcro_defin;
	}
}

/**
 * Processes lines inside a macro or regular lines. Checks if a line corresponds to a defined macro
 * and writes its content to the output assembly file or writes the line directly to the file.
 *
 * @param count_line The current line number being processed.
 * @param as_name The name of the source assembly file.
 * @param line The line being processed.
 * @param order_tree A pointer to the tree containing instruction, directive, and macro names.
 * @param am_file A pointer to the output assembly file.
 * @return SUCCESS if the line is successfully processed, unlegal_mcro_call if there's an issue with a macro call.
 */
static int enter_mcro_or_file(int count_line, char* as_name, char* line, Tree* order_tree, FILE* am_file) {
	int i , result;
	char first[MAX_LINE_LEN], second[MAX_LINE_LEN];
	char error[200];
	Mcro* found_mcro;
	result = sscanf(line, "%s %s", first, second);
	if (str_exist_type(first, order_tree) == mcro) {
		if (result == 2) {
			sprintf(error, "call to a macro requires a separate line.");
			print_error_or_worning(count_line, E, as_name, error);
			return unlegal_mcro_call;
		}
		found_mcro = str_exist(first, order_tree);
		i = 0;
		while (found_mcro->content[i] != NULL) {/*put the mcro syntax*/
			fprintf(am_file, found_mcro->content[i]);
			fprintf(am_file, "\n");
			i++;
		}
	}
	else {
		fprintf(am_file, "%s\n", line);
	}
	return SUCCESS;
}

/**
 * Adds a line of content to a Mcro structure, which represents the content of a defined macro.
 *
 * @param mcro A pointer to the Mcro structure where the line will be added.
 * @param new_line The new line of content to add.
 * @param num_lines The current number of lines in the Mcro content.
 * @return SUCCESS if the line is successfully added, MEMORY_ERROR if there's a memory allocation failure.
 */
int add_line_to_mcro(Mcro* mcro, const char* new_line, int num_lines) {
	char** new_content;
	new_content  = (char**)realloc(mcro->content, (num_lines + 3) * sizeof(char*));
	if (new_content == NULL) {
		printf("Memory allocation failed.\n");
		return MEMORY_ERROR;
	}
	new_content[num_lines] = _strdup(new_line);
	new_content[num_lines + 1] = '\n';
	new_content[num_lines + 1] = NULL;
	mcro->content = new_content;
	return SUCCESS;
}

/**
 * Determines the type of a line in the input assembly file, such as an empty line, documentation line,
 * macro definition, or regular instruction/directive line.
 *
 * @param line The line to analyze.
 * @return The appropriate value from the enum row_type representing the type of the line.
 */
int row_type(char* line) {
	/*if (*line = '\n');
		return empty_line;*/
	if (*line == ';')
		return docu_line;
	line = skip_spaces(line);
	if (*line == '\0')
		return empty_line;
	if (strncmp(line, "endmcro", strlen("endmcro")) == 0)
		return endmcro_defin;
	if (strncmp(line, "mcro ", strlen("mcro ")) == 0)
		return mcro_defin;
	else return regular_line;
}

/**
 * Validates the syntax of an endmcro directive line, ensuring there are no extra parameters after the directive.
 *
 * @param line The endmcro directive line to analyze.
 * @return legal_endmcro_defin if the directive syntax is valid, unlegal_endmcro_defin if the syntax is invalid.
 */
int endmcro_definition(char* line) {
	line = skip_word(line);
	line = skip_spaces(line);
	if (line == NULL ||*line == '\0')
		return legal_endmcro_defin;
	else
		return unlegal_endmcro_defin;
}

/**
 * Opens source and destination files based on their respective extensions, and returns the name of the source file.
 * Handles potential errors when opening files.
 *
 * @param file_base_name The base name of the file.
 * @param source_file A pointer to the source file.
 * @param destination_file A pointer to the destination file.
 * @param source_ext The extension of the source file.
 * @param destination_ext The extension of the destination file.
 * @param source_file_name_ptr A pointer to store the name of the source file.
 * @return SUCCESS if the files are successfully opened, REAL_ERROR if there's an error opening the files.
 */
int open_files(char* file_base_name, FILE** source_file, FILE** destination_file, char* source_ext, char* destination_ext, char** source_file_name_ptr) {
	char* source_file_name;
	char* destination_file_name;
	char error[MAX_ERROR_LEN];

	source_file_name = malloc(strlen(file_base_name) + strlen(source_ext) + 1);/*add one to '\0'*/
	destination_file_name = malloc(strlen(file_base_name) + strlen(destination_ext) + 1);
	if (source_file_name == NULL || destination_file_name == NULL) {
		return REAL_ERROR; 
	}

	strcpy(source_file_name, file_base_name);
	strcat(source_file_name, source_ext);
	strcpy(destination_file_name, file_base_name);
	strcat(destination_file_name, destination_ext);

	*source_file_name_ptr = source_file_name;

	*source_file = fopen(source_file_name, "r");
	*destination_file = fopen(destination_file_name, "w");
	if (*source_file == NULL || *destination_file == NULL) {
		sprintf(error, "attempt to open file X failed.");
		print_error_or_worning(ERROR_OPEN_FILE, E, "C103", error);
		free(source_file_name);
		free(destination_file_name);
		return REAL_ERROR; /* Error opening files*/
	}
	free(destination_file_name);
	return SUCCESS;
}

int preprocessor(const char* file_base_name, Tree* order_tree) {
	FILE* as_file;
	FILE* am_file;
	char* as_file_name, line[MAX_LINE_LEN + 5], error[MAX_ERROR_LEN];
	int rowType, flag, num_lines, count_line;
	Mcro* mcro;
	size_t len;
	if (open_files(file_base_name, &as_file, &am_file, as_file_ext, am_file_ext, &as_file_name) == REAL_ERROR)
		return REAL_ERROR;
	flag = to_file;
	num_lines = 0;
	count_line = 0;
	mcro = (Mcro*)mcro();
	while (fgets(line, sizeof(line), as_file)) {
		count_line++;
		if (strlen(line) >= MAX_LINE_LEN) {
			sprintf(error, "line length exceeds 80 characters.");
			print_error_or_worning(count_line, E, as_file_name, error);
			flag = error_file;
		}
		len = strlen(line);
		if (len > 0 && line[len - 1] == '\n') {
			line[len - 1] = '\0';
		}
		rowType = row_type(line);
		if (rowType == empty_line || rowType == docu_line)
			continue;
		if (flag != error_file) {
			if (rowType == regular_line) {
				if (flag == to_file) {
					if (enter_mcro_or_file(count_line, as_file_name, line, order_tree, am_file) == unlegal_mcro_call)
						flag = error_file;
				}
				if (flag == to_mcro) {
					if (add_line_to_mcro(mcro, line, num_lines) == MEMORY_ERROR) {
						flag = MEMORY_ERROR;
						break;
					}
					num_lines++;
				}
			}
			if (rowType == mcro_defin) {
				rowType = mcro_definition(count_line, as_file_name, line, order_tree, &mcro);
				if (rowType == MEMORY_ERROR) {
					flag = MEMORY_ERROR;
					break;
				}
				if (rowType == unlegal_mcro_defin)
					flag = error_file;
				else {
					flag = to_mcro;
					num_lines = 0;
				}
			}
			if (rowType == endmcro_defin) {
				rowType = endmcro_definition(line);
				if (row_type == unlegal_endmcro_defin)
					flag = error_file;
				else {
					flag = to_file;
				}
			}
		}
		else {
			if (rowType == mcro_defin) {
				rowType = mcro_definition(count_line, as_file_name, line, order_tree, &mcro);
				if (rowType == MEMORY_ERROR) {
					flag = MEMORY_ERROR;
					break;
				}
			}
			else if (rowType == endmcro_defin) {
				endmcro_definition(line);
			}
		}
	}
	fclose(as_file);
	fclose(am_file);
	free(mcro);
	if (flag == REAL_ERROR)
		return REAL_ERROR;
	return SUCCESS;
}