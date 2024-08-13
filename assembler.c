#define _CRT_SECURE_NO_WARNINGS
#include "tree.h"
#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"
#include "lexer.h"
#include "vector.h"
#include <stdio.h>
#include "preprosesor.h"
#define reset_tree(tree) for (i = 0; i < NUM_TREE_PTR; i++) tree->next[i] = NULL;
#define chak_memory(v) if (v == NULL) return MEMORY_ERROR;
#define BASE_ADDRESS 100
#define MIN_PLACES 10

/**
 * @brief A data structure representing a symbol.
 */
typedef struct symbol {
	enum {
		sym_extern = 16,
		sym_entry,
		sym_data,
		sym_code,
		entry_sym_data,
		entry_sym_code
	} sym_type; /**< Type of the symbol. */
	unsigned int address; /**< Address of the symbol. */
	unsigned int line_def; /**< Line where the symbol is defined. */
	char sym_name[MAX_LABLE_LEN]; /**< Name of the symbol. */
} Symbol;

/**
 * @brief A data structure representing a missing symbol.
 */
typedef struct mising_symbol {
	unsigned int* mechin_word; /**< Pointer to machine word. */
	unsigned int IC; /**< Instruction Counter. */
	unsigned int line; /**< Line number. */
	char mis_sym_name[MAX_LABLE_LEN]; /**< Name of the missing symbol. */
} Mis_symbol;

/**
 * @brief A data structure containing various vectors and a symbol tree.
 */
typedef struct ptr_package {
	Vector* mis_symbol; /**< Vector of missing symbols. */
	Vector* code_section; /**< Vector of code section. */
	Vector* data_section; /**< Vector of data section. */
	Vector* symbol_table; /**< Vector of symbol table. */
	Tree* symbol_table_lookup; /**< Symbol tree lookup. */
} Ptr_pack;

static void* contractor_mis_sym(const void* to_copy) {
	void* result = malloc(sizeof(Mis_symbol));
	if (result != NULL) {
		memcpy(result, to_copy, sizeof(Mis_symbol));
	}
	return result;
}

static void* contractor_mechin_word(const void* to_copy) {
	void* result = malloc(sizeof(unsigned int));
	if (result != NULL) {
		memcpy(result, to_copy, sizeof(unsigned int));
	}
	return result;
}

static void* contractor_symbol_table(const void* to_copy) {
	void* result = malloc(sizeof(Symbol));
	if (result != NULL) {
		memcpy(result, to_copy, sizeof(Symbol));
	}
	return result;
}

void destructor_ptr_package(Ptr_pack** ptr_pac) {
	free_vector(&((*ptr_pac)->code_section));
	free_vector(&((*ptr_pac)->data_section));
	free_vector(&((*ptr_pac)->mis_symbol));
	free_vector(&((*ptr_pac)->symbol_table));
	free_vector(&((*ptr_pac)->symbol_table_lookup));
	free((*ptr_pac));
}

/**
 * @brief Convert data from section into base64 and write to a file.
 *
 * @param section Vector representing a section of data.
 * @param ob_file Output file for base64 data.
 */
void convert_to_base64(Vector* section, FILE* ob_file) {
	char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char base64_str[2];
	int first_mask;
	int second_mask;
	unsigned int** first_v, ** last_v;
	if (section->count_item == 0)
		return;
	first_v = first_vector(section);
	last_v = last_vector(section);
	for (first_mask = 0x3F, second_mask = 0xFC0; first_v <= last_v; first_v++) {
		printf("%c\n", base64_chars[(**first_v) & first_mask]);
		base64_str[0] = base64_chars[(**first_v) & first_mask];
		base64_str[1] = base64_chars[((**first_v) & second_mask) >> 6];
		printf("%c\n", base64_chars[((**first_v) & second_mask) >> 6]);
		printf("%c%c\n", base64_str[1], base64_str[0]);
		fprintf(ob_file, "%c%c\n", base64_str[1], base64_str[0]);
	}
}

/**
 * @brief Create an object file from the Ptr_pack structure.
 *
 * @param base_name Base file name.
 * @param ptr_pac Pointer to Ptr_pack structure.
 * @return Returns SUCCESS on success, REAL_ERROR on real error.
 */
int make_object_file(char* base_name, Ptr_pack* ptr_pac) {
	char* full_name;
	FILE* ob_file;
	if (open_file(base_name, "w", ".ob", &full_name, &ob_file) == REAL_ERROR) {
		return REAL_ERROR;
	}
	fprintf(ob_file, "%d %d\n", ptr_pac->code_section->count_item, ptr_pac->data_section->count_item);
	convert_to_base64(ptr_pac->code_section, ob_file);
	convert_to_base64(ptr_pac->data_section, ob_file);
	return SUCCESS;
}

/**
 * @brief Create a Ptr_pack structure.
 *
 * @return Pointer to the created Ptr_pack structure, NULL on memory allocation error.
 */
Ptr_pack* craet_ptr_package() {
	int i = 0;
	Ptr_pack* ptr_pack = (Ptr_pack*)malloc(sizeof(Ptr_pack));
	Tree* symbol_table_lookup = (Tree*)malloc(sizeof(Tree));
	Vector* data_section = vector(contractor_mechin_word, destructor);
	Vector* code_section = vector(contractor_mechin_word, destructor);
	Vector* symbol_table = vector(contractor_symbol_table, destructor);
	Vector* mis_symbol = vector(contractor_mis_sym, destructor);
	if (ptr_pack == NULL || symbol_table_lookup == NULL || data_section == NULL ||
		code_section == NULL || symbol_table == NULL || mis_symbol == NULL) {
		print_error_or_worning(0, R, "", "memory allocation error");
		return NULL;
	}
	reset_tree(symbol_table_lookup);
	ptr_pack->mis_symbol = mis_symbol;
	ptr_pack->code_section = code_section;
	ptr_pack->data_section = data_section;
	ptr_pack->symbol_table = symbol_table;
	ptr_pack->symbol_table_lookup = symbol_table_lookup;
	return ptr_pack;
}

/**
 * @brief Inserts a symbol into the symbol table and handles related actions.
 *
 * @param line The line number where the symbol is encountered.
 * @param name_file The name of the file being processed.
 * @param ast The AST node representing the symbol.
 * @param ptr_pack The pointer to Ptr_pack structure.
 * @return Returns SUCCESS on success, ERROR on error, MEMORY_ERROR on memory allocation error.
 */
static int insert_sym(int line, char* name_file, AST* ast, Ptr_pack* ptr_pack) {
	int type;
	Symbol* find_sym;
	void* result;
	Symbol local_sym = { 0 };
	char error_print[200];
	int row_type = ast->row_type;
	find_sym = str_exist(ast->lable_name, ptr_pack->symbol_table_lookup);
	if (find_sym) {
		if (find_sym->sym_type == sym_entry) {
			if (row_type == instruction) {
				find_sym->sym_type = entry_sym_code;
				find_sym->address = ptr_pack->code_section->count_item + BASE_ADDRESS;// IC;
			}
			else {
				find_sym->sym_type = entry_sym_data;
				find_sym->address = ptr_pack->data_section->count_item + BASE_ADDRESS;// DC;

			}
			find_sym->line_def = line;
			return SUCCESS;
		}
		else {
			if (find_sym->sym_type == sym_extern) {
				sprintf(error_print, "The symbol %s has already been defined as extern", ast->lable_name, find_sym->line_def);
			}
			else {
				sprintf(error_print, "the symbol %s has already been defined in line %d ", ast->lable_name, find_sym->line_def);
			}
			print_error_or_worning(line, ERROR, name_file, error_print);
			return ERROR;
		}
	}
	else {
		if (row_type == dir_entry || row_type == dir_extern) {
			sprintf(error_print, "the %s symbol is ignored.", ast->lable_name);
			print_error_or_worning(line, W, name_file, error_print);
			return SUCCESS;
		}
		if (row_type == instruction) {
			local_sym.sym_type = sym_code;
			local_sym.address = ptr_pack->code_section->count_item + BASE_ADDRESS;// IC;
			type = sym_code;
		}
		else {
			local_sym.sym_type = sym_data;
			local_sym.address = ptr_pack->data_section->count_item + BASE_ADDRESS;// DC
			type = sym_data;

		}
		strcpy(local_sym.sym_name, ast->lable_name);
		local_sym.line_def = line;
		result = insert_vector(&local_sym, ptr_pack->symbol_table);
		chak_memory(result);
		insert_str(ast->lable_name, ptr_pack->symbol_table_lookup, result, type);
		return SUCCESS;
	}
	return SUCCESS;
}

/**
 * @brief Process an instruction AST node and add its information to the code section.
 *
 * @param line Line number.
 * @param ast AST structure containing information about the instruction.
 * @param ptr_pack Pointer to Ptr_pack structure.
 * @return Returns SUCCESS on success, MEMORY_ERROR on memory allocation error.
 */
int take_care_inst(int line, AST* ast, Ptr_pack* ptr_pack) {
	unsigned int mechin_word, i;
	unsigned int* inserted_word;
	mechin_word = ast->details.inst.inst_type << 5;
	mechin_word |= ast->details.inst.operand_options[0] << 9;
	mechin_word |= ast->details.inst.operand_options[1] << 2;
	chak_memory(insert_vector(&mechin_word, ptr_pack->code_section));

	if (ast->details.inst.operand_options[0] == operand_register && ast->details.inst.operand_options[1] == operand_register) {
		mechin_word = ast->details.inst.operand[1].register_num << 2;
		mechin_word |= ast->details.inst.operand[0].register_num << 7;
		chak_memory(insert_vector(&mechin_word, ptr_pack->code_section));
	}
	else {
		Mis_symbol mis_sym;
		for (i = 0; i < 2; i++) {
			switch (ast->details.inst.operand_options[i])
			{
			case operand_register:
				mechin_word = ast->details.inst.operand[i].const_num << (7 - (i * 5));
				break;
			case operand_const_num:
				mechin_word = ast->details.inst.operand[i].const_num << 2;
				break;
			case operand_lable:
				strcpy(mis_sym.mis_sym_name, ast->details.inst.operand[i].lable);
				mis_sym.line = line;
				mis_sym.IC = ptr_pack->mis_symbol->count_item;
				break;
			}
			if (ast->details.inst.operand_options[i] != operand_non) {
				inserted_word = insert_vector(&mechin_word, ptr_pack->code_section);
				if (inserted_word == NULL)
					return MEMORY_ERROR;
				if (ast->details.inst.operand_options[i] == operand_lable) {
					mis_sym.mechin_word = inserted_word;
					mis_sym.IC = ptr_pack->code_section->count_item + BASE_ADDRESS;
					chak_memory(insert_vector(&mis_sym, ptr_pack->mis_symbol));
				}
			}
		}
	}
	return SUCCESS;
}

/**
 * @brief Process a directive AST node and add its information to the appropriate section.
 *
 * @param file_name Name of the file.
 * @param line Line number.
 * @param ast AST structure containing information about the directive.
 * @param ptr_pac Pointer to Ptr_pack structure.
 * @return Returns SUCCESS on success, ERROR on error, MEMORY_ERROR on memory allocation error.
 */
int take_care_dir(char* file_name, int line, AST* ast, Ptr_pack* ptr_pac) {
	int i, flag_print, flag;
	char* temp;
	unsigned int mechin_word;
	Symbol* find_sym, corent_sym;
	void* result;
	char error_print[200];
	switch (ast->details.dir.dir_type) {
	case dir_data:
		for (i = 0; i < ast->details.dir.count_par; i++) {
			mechin_word = ast->details.dir.dir_operand.data[i];
			if (insert_vector(&mechin_word, ptr_pac->data_section) == NULL)
				return MEMORY_ERROR;
		}
		break;
	case dir_string:
		temp = ast->details.dir.dir_operand.string;
		while (*temp != '\0') {
			mechin_word = *temp;
			if (insert_vector(&mechin_word, ptr_pac->data_section) == NULL)
				return MEMORY_ERROR;
			temp++;
		}
		mechin_word = 0;
		if (insert_vector(&mechin_word, ptr_pac->data_section) == NULL)
			return MEMORY_ERROR;
		break;
	case dir_extern: case dir_entry:
		for (i = 0; i < ast->details.dir.count_par; i++) {
			find_sym = str_exist(ast->details.dir.dir_operand.lable_name[i], ptr_pac->symbol_table_lookup);
			if (find_sym) {
				if (ast->details.dir.dir_type == dir_extern) {
					switch (find_sym->sym_type)
					{
					case sym_extern:
						sprintf(error_print, "redefinition, the symbol %s has already been defined as extern.", find_sym->sym_name);
						print_error_or_worning(line, W, file_name, error_print);
						continue;
					case sym_entry:
						sprintf(error_print, "labels '%s' has already been defined as entry in line %d", find_sym->sym_name, find_sym->line_def);
					case entry_sym_code:
					case entry_sym_data:
					case sym_code:
					case sym_data:
						sprintf(error_print, "labels '%s' has already been defined in line %d", find_sym->sym_name, find_sym->line_def);
						break;

					}
					print_error_or_worning(line, E, file_name, error_print);
					return ERROR;/*all case ecsept dir_extern*/
				}
				if (ast->details.dir.dir_type == dir_entry) {
					switch (find_sym->sym_type)
					{
					case sym_extern:
						sprintf(error_print, "labels '%s' has already been defined as extern in line %d.", find_sym->sym_name, find_sym->line_def);
						print_error_or_worning(line, E, file_name, error_print);
						return ERROR;
						break;
					case sym_entry:
						sprintf(error_print, "redefinition, the symbol '%s' has already been defined as entry in line %d.", find_sym->sym_name, find_sym->line_def);
						print_error_or_worning(line, W, file_name, error_print);
						break;
					case entry_sym_code:
					case entry_sym_data:
						sprintf(error_print, "redefinition, the symbol '%s' has already been defined as entry.", find_sym->sym_name);
						print_error_or_worning(line, W, file_name, error_print);
						break;
					case sym_code:
					case sym_data:
						if (find_sym->sym_type == sym_code) {
							find_sym->sym_type = entry_sym_code;
						}
						else {
							find_sym->sym_type = entry_sym_data;
						}
					}
				}
			}
			else {
				corent_sym.sym_type = ast->details.dir.dir_type;
				corent_sym.address = 0;
				corent_sym.line_def = line;
				strcpy(corent_sym.sym_name, ast->details.dir.dir_operand.lable_name[i]);
				result = insert_vector(&corent_sym, ptr_pac->symbol_table);
				if (result == NULL)
					return MEMORY_ERROR;
				insert_str(corent_sym.sym_name, ptr_pac->symbol_table_lookup, result, lable);
			}
		}
	}
	return SUCCESS;
}

/**
 * @brief Process entry directives and write entries to an entry file.
 *
 * @param flag Flag indicating errors.
 * @param base_name Base file name.
 * @param am_name Assembly file name.
 * @param ptr_pac Pointer to Ptr_pack structure.
 * @return Returns SUCCESS on success, REAL_ERROR on real error.
 */
int take_care_entry(int* flag, const char* base_name, const char* am_name, Ptr_pack* ptr_pac) {
	Symbol** first_v = first_vector(ptr_pac->symbol_table);
	Symbol** last_v = last_vector(ptr_pac->symbol_table);
	char error[200];
	FILE* ent_file;
	char* ent_file_name;
	int output_flag = no_output;
	int IC = ptr_pac->code_section->count_item;
	if (ptr_pac->symbol_table->count_item == 0) {
		return SUCCESS;
	}
	if (open_file(base_name, "w", ent_file_ext, &ent_file_name, &ent_file) == REAL_ERROR) {
		return REAL_ERROR;
	}
	for (; first_v <= last_v; first_v++) {
		switch ((*first_v)->sym_type) {
		case sym_entry:
			sprintf(error, "an entry label '%s' was declared but not defined in file.", (*first_v)->sym_name);
			print_error_or_worning((*first_v)->line_def, E, am_name, error);
			*flag = ERROR;
			break;
		case entry_sym_code:
			fprintf(ent_file, "%s	%d\n", (*first_v)->sym_name, (*first_v)->address);
			output_flag = output;
			break;
		case entry_sym_data:
			fprintf(ent_file, "%s	%d\n", (*first_v)->sym_name, (*first_v)->address + IC);
			output_flag = output;
		case sym_data:
			(*first_v)->address += IC;
			break;
		}
	}
	if (*flag == ERROR) {
		sprintf(error, "due to errors internal file '%s' wIll deleted", ent_file_name);
		print_error_or_worning(WARNING_DELET, W, "", error);
		output_flag = no_output;
	}
	if (output_flag == no_output) {
		if (remove(ent_file_name) != 0) {
			fprintf(stderr, "File deletion failed.\n");
		}
		fprintf(stderr, "File deleted successfully.\n");
	}
	fclose(ent_file);
	return SUCCESS;
}

/**
 * @brief Process missing symbols and write externs to an extern file.
 *
 * @param flag Flag indicating errors.
 * @param base_name Base file name.
 * @param am_name Assembly file name.
 * @param ptr_pac Pointer to Ptr_pack structure.
 * @return Returns SUCCESS on success, REAL_ERROR on real error.
 */
int complit_extern_mis_sym(int* flag, const char* base_name, const char* am_name, Ptr_pack* ptr_pac) {
	if (ptr_pac->mis_symbol->count_item == 0) {/*ther is no mising simbol-ther are no call to symbol->ther are no call to extern symbol*/
		return SUCCESS;
	}
	Mis_symbol** first_v = first_vector(ptr_pac->mis_symbol);
	Mis_symbol** last_v;
	char error[200];
	Symbol* found_sym;
	char* ext_name;
	FILE* ext_file;
	int count_item;
	int output_flag = no_output;
	if (open_file(base_name, "w", ext_file_ext, &ext_name, &ext_file) == REAL_ERROR) {
		return REAL_ERROR;
	}
	for (last_v = last_vector(ptr_pac->mis_symbol); first_v <= last_v; first_v++) {
		found_sym = str_exist((*first_v)->mis_sym_name, ptr_pac->symbol_table_lookup);
		if (found_sym) {
			if (found_sym->sym_type == sym_entry) {
				sprintf(error, "call to an undefined entry label '%s'.", (*first_v)->mis_sym_name);
				print_error_or_worning((*first_v)->line, E, am_name, error);
				*flag = ERROR;
				continue;
			}
			else {
				*((*first_v)->mechin_word) = found_sym->address << 2;
				if (found_sym->sym_type == sym_extern) {
					*((*first_v)->mechin_word) = 1;
					fprintf(ext_file, "%s	%d\n", found_sym->sym_name, (*first_v)->IC-1);
					output_flag = output;
				}
				else
					*((*first_v)->mechin_word) |= 2;
			}
		}
		else {
			sprintf(error, "call to an undefined label '%s'.", (*first_v)->mis_sym_name);
			print_error_or_worning((*first_v)->line, E, am_name, error);
			*flag = ERROR;
			continue;
		}
	}
	if (*flag == ERROR) {
		sprintf(error, "due to errors file '%s' will deleted", ext_name);
		print_error_or_worning(WARNING_DELET, W, "", error);
		output_flag = no_output;
	}
	if (output_flag == no_output) {
		if (remove(ext_name) != 0) {
			fprintf(stderr, "File deletion failed.\n");
		}
		fprintf(stderr, "File deleted successfully.\n");
	}
	fclose(ext_file);
	return SUCCESS;
}


static int assembler_compiler(Tree* order_tree, char* base_file_name) {
	AST* ast;
	Ptr_pack* ptr_pac = craet_ptr_package();
	char line[MAX_LINE_LEN];
	unsigned int count_line = 1;
	int flag, result;
	FILE* am_file;
	char* am_file_name;
	flag = SUCCESS;
	if (open_file(base_file_name, "r", am_file_ext, &am_file_name, &am_file) == REAL_ERROR)
		return REAL_ERROR;
	while (fgets(line, sizeof(line), am_file)) {
		ast = lexer(order_tree, line);
		if (ast == NULL) {
			flag = MEMORY_ERROR;
			break;
		}
		else if (ast->row_type == ERROR) {
			print_error_or_worning(count_line, ERROR, am_file_name, ast->error);
			flag = ERROR;
		}
		else if (*(ast->lable_name) != '\0') {
			result = insert_sym(count_line, am_file_name, ast, ptr_pac);
			if (result == ERROR)
				flag = ERROR;
			if (result == MEMORY_ERROR) {
				flag = MEMORY_ERROR;
				break;
			}
		}
		if (ast->row_type == instruction) {
			if (take_care_inst(count_line, ast, ptr_pac) == MEMORY_ERROR) {
				flag = MEMORY_ERROR;
				break;
			}
		}
		else if (ast->row_type == directive) {
			result = take_care_dir(am_file_name, count_line, ast, ptr_pac);
			if (result == ERROR)
				flag = ERROR;
			if (result == MEMORY_ERROR) {
				flag = MEMORY_ERROR;
				break;
			}
		}
		if ((ptr_pac->code_section->count_item + ptr_pac->data_section->count_item) >= MAX_ADDRESS) {
			print_error_or_worning(count_line, E, am_file_name, "deviation from the memory buffers");
			flag == ERROR;
		}
		count_line++;
		
	}
	fclose(am_file);
	if (flag == MEMORY_ERROR) {
		destructor_ptr_package(&ptr_pac);
		return REAL_ERROR;
	}

	if (take_care_entry(&flag, base_file_name, am_file_name, ptr_pac) == REAL_ERROR) {
		destructor_ptr_package(&ptr_pac);
		return REAL_ERROR;
	}
	
	if (complit_extern_mis_sym(&flag, base_file_name, am_file_name, ptr_pac) == REAL_ERROR) {
		destructor_ptr_package(&ptr_pac);
		return REAL_ERROR;
	}
	if (flag == ERROR) {
		destructor_ptr_package(&ptr_pac);
		return ERROR;
	}
	flag = make_object_file(base_file_name, ptr_pac) == REAL_ERROR;
	destructor_ptr_package(&ptr_pac);
	return flag;
}




int assembler(int file_count, char** name_file) {
	int i, index;
	int result;
	Tree* order_tree = (Tree*)malloc(sizeof(Tree));
	for (index = 0; index < file_count; index++) {
		reset_tree(order_tree);
		insert_der_inst(order_tree);
		result = preprocessor(name_file[index], order_tree);
		if (result == ERROR)
			continue;
		if (result == REAL_ERROR)
			break;
		result = assembler_compiler(order_tree, name_file[index]);
		if (result == REAL_ERROR)
			break;
	}
	free(order_tree);
	return 0;
}