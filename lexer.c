#define _CRT_SECURE_NO_WARNINGS
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "function.h"
#define LABLE_ERROR -10

/**
 * @struct Order
 * Represents an order for an instruction.
 */
struct Order {
	int operand;            /**< Indicator if operand is needed represent as the opisit*/
	char* type_operand;     /**< Type of operand */
	int N;                  /**< N flag */
	int L;                  /**< L flag */
	int R;                  /**< R flag */
};

/**
 * @var inst_opernd
 * Array representing operand information for instructions and directives.
 */
const struct Order inst_opernd[3][16] = {
	{
	{off,"number, label or register",on,on,on},
	{off,"number, label or register",on,on,on},
	{off,"number, label or register",on,on,on},
	{off,"number, label or register",on,on,on},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{off,"label",off,on,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
},
{
	{off,"label or register",off,on,on},
	{off,"number, label or register",on,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"label or register",off,on,on},
	{off,"number, label or register",on,on,on},
	{off,"label or register",off,on,on},
	{on,"",off,off,off},
	{on,"",off,off,off},

},
{
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
	{on,"",off,off,off},
}
};

/**
 * Checks if the given string represents a valid number.
 *
 * @param operand The string to check.
 * @return SUCCESS if the string is a valid number, IS_ERROR if it's an invalid number, ERROR otherwise.
 */
int is_num(char* operand) {
	if (*operand == '+' || *operand == '-')
		operand++;
	while (*operand) {
		if (!isdigit(*operand)) {
			break; // Not a number
		}
		operand++;
	}
	if (*operand == '.') {
		while (*operand) {
			if (!isdigit(*operand)) {
				return ERROR; // Not a number
			}
			operand++;
		}
		//sprintf((*ast)->error, "Invalid number - a number of the wrong type");
		return IS_ERROR;
	}
	if (*operand == '\0') {
		return SUCCESS;
	}
	return ERROR;

}

/**
 * Checks if the given character is a comma and handles various cases.
 *
 * @param c The character to check.
 * @param flag The flag indicating the context.
 * @param ast The AST structure.
 * @return SUCCESS if the character is as expected, ERROR otherwise.
 */
int is_comma(char c, int flag, AST* ast) {
	if (c == ',') {
		if (flag == on)
			return  SUCCESS;
		if (flag == duplicateChecker)
			sprintf(ast->error, "Multiple consecutive commas\n");
		else sprintf(ast->error, "Illegal comma\n");
		return ERROR;
	}
	if (flag == on) {
		sprintf(ast->error, "Missing comma\n");
		return ERROR;
	}
	return SUCCESS;
}

/**
 * Checks if the given line is at its end or contains only spaces.
 *
 * @param line The line to check.
 * @param flag The flag indicating the context.
 * @param ast The AST structure.
 * @return SUCCESS if the line is at its end, ERROR otherwise.
 */
int is_end(char* line, int flag, AST* ast) {
	if (line == NULL || *line == '\0') {
		if (flag == on)
			return SUCCESS;
		else {/*flag == off*/
			sprintf(ast->error, "missing parameters");
			return ERROR;
		}
	}
	if (isspace(*line)) {
		line = skip_spaces(line);
		return is_end(line, flag, ast);
	}
	if (flag == on) {
		sprintf(ast->error, "extra parameters");
		return ERROR;
	}
	return SUCCESS;
}

/**
 * Checks if the given string has the potential to be a label.
 *
 * @param str The string to check.
 * @return SUCCESS if the string has the potential to be a label, ERROR otherwise.
 */
int label_potential(char* str) {
	char c;
	if (is_letters_and_numbers(str) == ERROR)
		return ERROR;
	c = *str;
	if (c < 'A' || c > 'Z' && c < 'a' || c > 'z')
		return ERROR;
	return SUCCESS;
}

/**
 * Checks if the given word is a legal name according to the order tree.
 *
 * @param word The word to check.
 * @param order_tree The tree containing language order.
 * @param ast The AST structure.
 * @return SUCCESS if the word is a legal name, ERROR otherwise.
 */
int legal_name(const char* word, Tree* order_tree, AST* ast) {
	int type;
	char c;
	type = str_exist_type(word, order_tree);
	if (type != ERROR) {
		if (type >= Inst_mov && type <= Inst_stop) {
			sprintf(ast->error, "illegal lable name, %s is alredy instruction name.", word);
		}
		else if (type >= dir_extern && type <= dir_data) {
			sprintf(ast->error, "illegal lable name, %s is alredy directive name.", word);
		}
		else if (type == mcro) {
			sprintf(ast->error, "illegal lable name, %s is alredy mcro name.", word);
		}
		return ERROR;
	}
	c = *word;
	if (c < 'A' || c > 'Z' && c < 'a' || c > 'z') {
		sprintf(ast->error, "illegal lable name, lable name have to begin whith letters.");
		return ERROR;
	}
	if (is_letters_and_numbers(word) == ERROR) {
		sprintf(ast->error, "illegal lable name, lable name must contain only letters and numbers.");
		return ERROR;
	}
	return SUCCESS;
}

/**
 * Splits a string into two parts based on a comma.
 *
 * @param str The string to split.
 * @param str1 The pointer to store the first part of the split.
 * @param str2 The pointer to store the second part of the split.
 * @return 2 if both parts are present, 1 if only one part is present, 0 otherwise.
 */
int split_str3(char* str, char** str1, char** str2, AST* ast) {
	int i;
	str = skip_spaces(str);
	if (is_end(str, off, ast) == ERROR)
		return ERROR;
	static char first[MAX_LABLE_LEN];
	i = 0;

	while (*str != '\0' && *str != ',' && !isspace(*str)) {
		first[i] = *str; 
		i++;
		str++;
	}
	first[i] = '\0';
	*str1 = first;  
	str = skip_spaces(str);

	if (*str == '\0')
		return 1;

	else if (*str == ',') {
		str++;
		if (is_comma(*str, duplicateChecker, ast) == ERROR)
			return ERROR;
		*str2 = str;
		return 2;
	}
	return 0; /*we never came here*/
}

/**
 * Checks if the given string is a string definition.
 *
 * @param str The string to check.
 * @param ast The AST structure.
 * @return A pointer to the AST node if successful, NULL otherwise.
 */
 AST* is_string(char* str, AST* ast) {
	int i;
	if (*str != '"') {
		sprintf(ast->error, "a string definition must begin with the character \" ");
		return error_ast(ast);
	}
	str++;

	static char first[MAX_LINE_LEN];
	i = 0;

	while (*str != '\0' && *str != '"') {
		first[i] = *str;
		if (*str < 32 || *str > 126) {
			sprintf(ast->error, "the character %c is invalid.", *str);
			return error_ast(ast);
		}
		str++;
		i++;
	}
	if (str[i] == '\0') {
		sprintf(ast->error, "missing \" after finishing string definition ");
		return error_ast(ast);
	}
	str++;
	if (is_end(str, on, ast) == ERROR)
		return error_ast(ast);
	first[i] = '\0';
	ast->details.dir.dir_operand.string = first;
	return ast;
}

/**
 * Checks and processes legal numbers or labels for directives.
 *
 * @param line The line to check.
 * @param ast The AST structure.
 * @param chak_operand The operand checker function.
 * @return A pointer to the AST node if successful, NULL otherwise.
 */
AST* legal_numbers_or_labels(char* line, AST* ast, void* chak_operand) {
	char* str1, *str2;
	int result = 0;
	int count_par = 0;
	str2 = line;
	while ((result = split_str3(str2, &str1, &str2, ast)) == 2) {
		if ((*(int (*)(char*, AST*, int))chak_operand)(str1, ast, count_par) == ERROR)
			return error_ast(ast);
		count_par++;
	}
	if (result == 1) {
		if ((*(int (*)(char*, AST*, int))chak_operand)(str1, ast, count_par) == ERROR)
			return error_ast(ast);
		else {
			count_par++;
			ast->details.dir.count_par = count_par;
			return ast;
		}
	}
	else return error_ast(ast);
}

/**
 * Checks and processes a label operand for directives.
 *
 * @param operand The operand to check.
 * @param ast The AST structure.
 * @param count_par The parameter count.
 * @return SUCCESS if successful, ERROR otherwise.
 */
int take_care_lable(char* operand, AST* ast, int count_par) {
	if (label_potential(operand) == ERROR) {
		sprintf(ast->error, "'%s' is not recognized, expecting a lable", operand);
		return ERROR;
	}
	ast->details.dir.dir_operand.lable_name[count_par] = _strdup(operand);
	return SUCCESS;
}

/**
 * Checks and processes a numeric operand for directives.
 *
 * @param operand The operand to check.
 * @param ast The AST structure.
 * @param count_par The parameter count.
 * @return SUCCESS if successful, ERROR otherwise.
 */
int take_care_num(char* operand, AST* ast, int count_par) {
	int result;
	result = is_num(operand);
	if (result == SUCCESS) {
		ast->details.dir.dir_operand.data[count_par] = atoi(operand);
		return SUCCESS;
	}
	else {
		if (result == ERROR)
			sprintf(ast->error, "'%s' is not recognized, expecting a number", operand);
		if (result == IS_ERROR)
			sprintf(ast->error, "invalid number '%s'", operand);
		return ERROR;
	}
}

/**
 * Checks and processes parameters for directives.
 *
 * @param line The line to check.
 * @param ast The AST structure.
 * @return A pointer to the AST node if successful, NULL otherwise.
 */
AST* chak_parm_dir(char* line, AST* ast) {
	if (is_end(line, off, ast) == ERROR)
		return error_ast(ast);
	line = skip_spaces(line);
	if (is_comma(*line, off, ast) == ERROR)
		return error_ast(ast);
	switch (ast->details.dir.dir_type) {
	case dir_extern:
	case dir_entry:
		return legal_numbers_or_labels(line, ast, take_care_lable);
		break;
	case dir_string:
		return is_string(line, ast);
		break;
	case dir_data:
		return legal_numbers_or_labels(line, ast, take_care_num);
	}
	return 0;
}

/**
 * Checks if the given operand is a register.
 *
 * @param operand The operand to check.
 * @return SUCCESS if the operand is a valid register, ERROR otherwise.
 */
int is_reg(char* operand) {
	if (*operand == '@') {
		operand++;
		if (*operand == 'r') {
			operand++;
			if (*operand <= '7' && *operand >= '0') {
				operand++;
				if (*operand == '\0')
					return SUCCESS;
				return IS_ERROR;
			}
			return IS_ERROR;
		}
	}
	return ERROR;
}

/**
 * Gets the type of operand for an instruction.
 *
 * @param operand The operand to check.
 * @param ast The AST structure.
 * @param time The operand time (0 or 1).
 * @return The operand type.
 */
int get_operand(char* operand, AST* ast, int time) {
	int operand_type;

	operand_type = is_num(operand);
	if (operand_type == SUCCESS) {
		int number = atoi(operand);
		if (inst_opernd[time][ast->details.inst.inst_type].N == on) {
			if (number < -512 || number > 511) {
				sprintf(ast->error, "invalid number '%s', expect a number between -512 to 511", operand);
				return IS_ERROR;
			}
		}
		return num;
	}
	if (operand_type == IS_ERROR) {
		if (inst_opernd[time - 1][ast->details.inst.inst_type].N == on) {
			sprintf(ast->error, "invalid number '%s'", operand);
			return IS_ERROR;
		}
		return ERROR;
	}

	operand_type = is_reg(operand);
	if (operand_type == SUCCESS)
		return reg;
	if (operand_type == IS_ERROR) {
		if (inst_opernd[time - 1][ast->details.inst.inst_type].R == on) {
			sprintf(ast->error, "'%s' register not recognized, in order to enter a register press @X where X is the number of the requested register (0-7)", operand);
			return IS_ERROR;
		}
		return ERROR;
	}

	operand_type = label_potential(operand);
	if (operand_type == SUCCESS)
		return lable;

	else return ERROR;
}

/**
 * Processes and validates instruction operands.
 *
 * @param operand The operand to process.
 * @param order_tree The tree containing language order.
 * @param ast The AST structure.
 * @param time The operand time (0 or 1).
 * @return A pointer to the AST node if successful, NULL otherwise.
 */
int take_care_operand(char* operand, Tree* order_tree, AST* ast, int time) {
	switch (get_operand(operand, ast, time))
	{
	case ERROR:
		sprintf(ast->error, "%s is undefined, expecting to %s", operand, inst_opernd[time][ast->details.inst.inst_type].type_operand);
		break;
	case num:
		if (inst_opernd[time][ast->details.inst.inst_type].N == off) {
			sprintf(ast->error, "invalid operand '%s' expected to %s", operand, inst_opernd[time][ast->details.inst.inst_type].type_operand);
			return ERROR;
		}
		else {
			ast->details.inst.operand_options[time] = operand_const_num;
			ast->details.inst.operand[time].const_num = atoi(operand);
			return SUCCESS;
		}
	case lable:
		if (legal_name(operand, order_tree, ast) == SUCCESS) {
			ast->details.inst.operand_options[time] = operand_lable;
			strcpy(ast->details.inst.operand[time].lable, operand);
			return SUCCESS;/*all the inst get opernd if the get samthing*/
		}
		else 
			return ERROR;
	case reg:
		if (inst_opernd[time][ast->details.inst.inst_type].R == off) {
			sprintf(ast->error, "invalid operand '%s' expected to %s", operand, inst_opernd[time - 1][ast->details.inst.inst_type].type_operand);
			return ERROR;
		}
		else {
			ast->details.inst.operand_options[time] = operand_register;
			ast->details.inst.operand[time].register_num = atoi(&operand[2]);
			return SUCCESS;
		}
		break;
	}
	return ERROR;/*in case of IS_ERROR*/
}

/**
 * Splits a string into two parts based on a space or comma.
 *
 * @param str The string to split.
 * @param str1 The pointer to store the first part of the split.
 * @param str2 The pointer to store the second part of the split.
 * @return 2 if both parts are present, 1 if only one part is present, 0 otherwise.
 */
int split_str2(char* str, char** str1, char** str2) {
	static char first[MAX_LABLE_LEN]; 
	int i;
	i = 0;
	while (str[i] != '\0' && str[i] != ',' && !isspace(str[i])) {
		first[i] = str[i];  
		i++;
	}
	first[i] = '\0';
	*str1 = first;  
	*str2 = (char*)&str[i]; 
	*str2 = skip_spaces(*str2);
	return 0;
}

/**
 * Checks and processes instruction operands.
 *
 * @param line The line to check.
 * @param order_tree The tree containing language order.
 * @param ast The AST structure.
 * @return A pointer to the AST node if successful, NULL otherwise.
 */
AST* chak_parm_inst(char* line, Tree* order_tree, AST* ast) {
	int inst_type, time;
	char* str1, *str2;
	inst_type = ast->details.inst.inst_type;
	if (is_end(line, inst_opernd[1][inst_type].operand, ast) == ERROR)/*ther is no inst thet have just sours opernd*/
		return error_ast(ast);
	if (inst_opernd[1][inst_type].operand == on) {
		ast->details.inst.operand_options[0] = operand_non;
		ast->details.inst.operand_options[1] = operand_non;
		return ast;
	}
	line = skip_spaces(line);
	if (is_comma(*line, off, ast) == ERROR)
		return error_ast(ast);
	
	split_str2(line, &str1, &str2);
	time = 0;
	if (inst_opernd[0][inst_type].operand == on) {
		ast->details.inst.operand_options[0] = operand_non;
		time = 1;
	}
	if (take_care_operand(str1, order_tree, ast, time) == ERROR) {
		return error_ast(ast);
	}
	time++;
	if (is_end(str2, inst_opernd[time][inst_type].operand, ast) == ERROR)
		return error_ast(ast);
	if (inst_opernd[time][inst_type].operand == on)
		return ast;
	if (is_comma(*str2, on, ast) == ERROR)
		return error_ast(ast);
	str2++;
	str2 = skip_spaces(str2);
	if (is_comma(*str2, duplicateChecker, ast) == ERROR)
		return error_ast(ast);
	split_str2(str2, &str1, &str2);
	if (take_care_operand(str1, order_tree, ast, time) == ERROR) {
		return error_ast(ast);
	}
	if (is_end(str2, on, ast) == ERROR)
		return error_ast(ast);
	return ast;
}

/**
 * Checks if the given string represents a directive.
 *
 * @param order_tree The tree containing language order.
 * @param word The word to check.
 * @param ast The AST structure.
 * @return SUCCESS if the word represents a directive, IS_ERROR if invalid, ERROR otherwise.
 */
int is_dir(Tree* order_tree, char* word, AST* ast) {
	int type;
	if (*word == '.') {
		word++;
		type = str_exist_type(word, order_tree);
		if (type != ERROR && type <= dir_data && type >= dir_extern) {
			ast->details.dir.dir_type = type;
			return SUCCESS;
		}
		else {
			sprintf(ast->error, "no directives found namde '%s'", word);
			return IS_ERROR;
		}
	}
	else {
		type = str_exist_type(word, order_tree);
		if (type != ERROR && type <= dir_data && type >= dir_extern) {
			sprintf(ast->error, "a dot is missing before the directives.");
			return IS_ERROR;
		}
		else
			return ERROR;
	}
}

/**
 * Checks if the given string represents an instruction.
 *
 * @param order_tree The tree containing language order.
 * @param word The word to check.
 * @param ast The AST structure.
 * @return SUCCESS if the word represents an instruction, ERROR otherwise.
 */
int is_inst(Tree* order_tree, char* word, AST* ast) {
	int type;
	type = str_exist_type(word, order_tree);
	if (type != ERROR && type <= Inst_stop && type >= Inst_mov) {
		ast->details.inst.inst_type = type;
		return SUCCESS;
	}
	else return ERROR;
}

/**
 * Checks if the given string represents a label definition.
 *
 * @param order_tree The tree containing language order.
 * @param str The string to check.
 * @param ast The AST structure.
 * @return SUCCESS if the string represents a label definition, IS_ERROR otherwise.
 */
int is_lable_definition(Tree* order_tree, char* str, AST* ast) {
	size_t len = strlen(str);
	if (str[len - 1] == ':') {
		str[len - 1] = '\0';
		if (legal_name(str, order_tree, ast) == SUCCESS) {
			ast->lable_name = str;
			return SUCCESS;
		}
		else
			return IS_ERROR;
	}
	else {
		if (legal_name(str, order_tree, ast) == SUCCESS) {
			sprintf(ast->error, "Mising a ':' right after the label %s", str);
			return IS_ERROR;
		}
		else
			return ERROR;
	}
}

/**
 * Determine the type of a string and perform corresponding actions.
 *
 * This function takes a string and processes it to determine its type
 * within the context of a given order_tree and AST. It checks if the
 * string is a directive, an instruction, or a label definition, and sets
 * the appropriate row_type in the AST structure.
 *
 * @param order_tree A pointer to the order tree structure.
 * @param str The input string to be analyzed.
 * @param ast A pointer to the AST structure to be updated.
 * @return An integer representing the type of the analyzed string.
 *         Returns directive, instruction, lable, ERROR, or LABLE_ERROR
 *         based on the type of the string and the analysis result.
 */

int str_type(Tree* order_tree, char* str, AST* ast) {
	int str_type;

	str_type = is_dir(order_tree, str, ast);
	if (str_type == SUCCESS) {
		ast->row_type = directive;
		return directive;
	}
	if (str_type == IS_ERROR) {
		return ERROR;
	}

	str_type = is_inst(order_tree, str, ast);
	if (str_type == SUCCESS) {
		ast->row_type = instruction;
		return instruction;
	}

	str_type = is_lable_definition(order_tree, str, ast);
	if (str_type == SUCCESS) {
		return lable;
	}
	if (str_type == IS_ERROR) {
		return LABLE_ERROR;
	}

	sprintf(ast->error, "The word '%s' is not recognized", str);
	return ERROR;
}

/**
 * Split a string into two parts at the first occurrence of ':'.
 *
 * This function takes a string and splits it into two parts, str1 and str2,
 * where str1 contains characters before the first ':' and str2 contains
 * characters after the ':' (including the ':'). It is used for separating
 * label definitions from the rest of the string.
 *
 * @param str The input string to be split.
 * @param str1 Pointer to a character pointer where the first part of the split
 *             string will be stored.
 * @param str2 Pointer to a character pointer where the second part of the split
 *             string will be stored.
 * @return An integer representing the result of the string splitting operation.
 *         Returns SUCCESS if the split is successful, or ERROR if the input
 *         string is too long or if no ':' is found.
 */
int split_str(const char* str, char** str1, char** str2) {
	static char lable[MAX_LABLE_LEN]; 
	int i;
	i = 0;
	while (str[i] != '\0' && str[i] != ':') {
		lable[i] = str[i];  

		if (i >= MAX_LABLE_LEN) {
			return ERROR;
		}
		i++;
	}

	if (str[i] == ':') {
		lable[i] = ':'; 
		i++;
	}
	lable[i] = '\0';
	*str1 = lable;  
	*str2 = (char*)&str[i];
	return SUCCESS;
}

/**
 * Determines the type of the first word in a line.
 *
 * @param order_tree The tree containing language order.
 * @param str The line to check.
 * @param ast The AST structure.
 * @return The type of the first word.
 */
int first_word_type(Tree* order_tree, char* str, AST* ast) {
	char* str1;
	char* str2;
	if (split_str(str, &str1, &str2) == ERROR) {
		sprintf(ast->error, "the word '%s' is to long", str1);
		return ERROR;
	}
	int str1_type = str_type(order_tree, str1, ast);
	if (str1_type == lable) {
		if (str2 == NULL || *str2 == '\0')
			return lable;

		int str2_type = str_type(order_tree, str2, ast);
		if (str2_type == instruction) {
			return instruction;
		}
		if (str2_type == directive) {
			sprintf(ast->error, "missing space, missing space between the label and directive");
			return ERROR;
		}
		else {
			strcpy(ast->error, "");
			sprintf(ast->error, "The word '%s' is not recognized", str);
			return ERROR;
		}
	}
	else
		return str1_type;
}

/**
 * Creates an error AST node.
 *
 * @param ast The AST node to mark as an error.
 * @return A pointer to the error AST node.
 */
AST* error_ast(AST* ast) {
	ast->row_type = ERROR;
	return ast;
}

AST* lexer(Tree* order_tree, char* line) {
	AST* ast;
	char first_word[MAX_LINE_LEN], secend_word[MAX_LINE_LEN];
	int word_type, num;
	size_t len;
	len = strlen(line);
	if (len > 0 && line[len - 1] == '\n') {
		line[len - 1] = '\0';
	}
	ast = (AST*)malloc(sizeof(AST)); 
	if (ast == NULL) {
		print_error_or_worning(ERROR_MEMORY_ALLOC, E, "C", "memory allocation failure.");
		return NULL;
	}
	num = sscanf(line, "%s", first_word);
	if (num == -1)
		return NULL;
	ast->lable_name = "\0";
	word_type = first_word_type(order_tree, first_word, ast);
	line = skip_word(line);
	if (word_type == ERROR || word_type == LABLE_ERROR)
		return error_ast(ast);
	if (word_type == lable) {
		num = sscanf(line, "%s", secend_word);
		if (num != -1) {
			word_type = str_type(order_tree, secend_word, ast);
			if (word_type == lable) {
				sprintf(ast->error, "'%s' is lable define, trying to define more than one label in a line", secend_word);
				return error_ast(ast);
			}
			if (word_type == LABLE_ERROR) {
				strcpy(ast->error, "");
				sprintf(ast->error, "The word '%s' is not recognized", secend_word);
				return error_ast(ast);
			}
			if (word_type == ERROR) {
				return error_ast(ast);
			}
		}
		else {
			sprintf(ast->error, "missing parameters");
			return error_ast(ast);
		}
		line = skip_word(line);
	}
	if (word_type == instruction) {
		return chak_parm_inst(line, order_tree, ast);
	}
	if (word_type == directive) {
		return chak_parm_dir(line, ast);
	}
	return NULL;/*we never dont came here*/
}