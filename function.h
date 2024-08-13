#ifndef FUNCTION_H
#define FUNCTION_H

#define IS_ERROR -5
#define ERROR -1
#define SUCCESS 1
#define MAX_LINE_LEN 81
#define MAX_LABLE_LEN 32
#define MEMORY_ERROR -100
#define REAL_ERROR -100
#define WARNING_DELET 405
#define SPACES_CHARECTER "\t\n\f\r\v\x20"
#define as_file_ext ".as"
#define am_file_ext ".am"
#define ent_file_ext ".ent"
#define ext_file_ext ".ext"
#define ERROR_OPEN_FILE 307
#define ERROR_MEMORY_ALLOC 507
#define MAX_ERROR_LEN 200

#include "tree.h"

int is_letters_and_numbers(const char* str);

char* skip_word(char* line);

char* skip_spaces(char* line);

void insert_der_inst(Tree*);

void print_error_or_worning(int, int, char*, char*);

void* contractor_contex(const void* to_copy);

void destructor(void* item);

//int open_file_with_name(const char*, const char*, const char*, char**, FILE**);
enum print_type {
	W,
	E,
	R
};

enum word_type {
	Inst_mov,
	Inst_cmp,
	Inst_add,
	Inst_sub,
	Inst_not,
	Inst_clr,
	Inst_lea,
	Inst_inc,
	Inst_dec,
	Inst_jmp,
	Inst_bne,
	Inst_red,
	Inst_prn,
	Inst_jsr,
	Inst_rts,
	Inst_stop,

	dir_extern,
	dir_entry,
	dir_string,
	dir_data,


	lable,
	mcro,
	instruction,
	directive,



	reg,
	num,
	mis

};

#endif
