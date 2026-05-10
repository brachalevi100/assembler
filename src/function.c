#define _CRT_SECURE_NO_WARNINGS
#include "function.h"
#include <ctype.h>  
#include <string.h> 
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#define COLOR_RESET "\033[0m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"



void destructor(void* item) {
    free(item);
}

int is_letters_and_numbers(const char* str) {
    while (*str != '\0') {
        if (!isalnum(*str)) {
            return ERROR;
        }
        str++;
    }
    return SUCCESS;
}

void print_error_or_worning(int line, int flag, char* name_file, char* to_print) {
    if (flag != R)
        fprintf(stderr, "%s:%d ", name_file, line);
    flag == E ? fprintf(stderr, COLOR_RED"error: "COLOR_RESET) : fprintf(stderr, COLOR_YELLOW"warning: "COLOR_RESET);
    fprintf(stderr, "%s\n", to_print);
}
int open_file(const char* base_name, const char* mode, const char* file_ent, char** full_name, FILE** file_ptr) {

    char* full_file_name = NULL;
    int result = 0;

    size_t base_len = strlen(base_name);
    size_t file_ent_len = strlen(mode);
    size_t file_name_len = base_len + file_ent + 1; 
    full_file_name = (char*)malloc(file_name_len);

    if (full_file_name) {
        
        snprintf(full_file_name, file_name_len, "%s%s", base_name, file_ent);

        *file_ptr = fopen(full_file_name, mode);
        if (*file_ptr) {
            
            *full_name = full_file_name;
            return SUCCESS;
        }
        else {
            free(full_file_name);
            REAL_ERROR;
        }
    }
    return REAL_ERROR;
}


char* skip_word(char* line) {
    line = skip_spaces(line);
    return strpbrk(line, SPACES_CHARECTER);
}

char* skip_spaces(char* line) {
    while (line != NULL && isspace(*line))
         line++;
    return line;
}


void insert_der_inst(Tree* order_tree) {
    insert_str("mov", order_tree, NULL, Inst_mov);
    insert_str("cmp", order_tree, NULL, Inst_cmp);
    insert_str("add", order_tree, NULL, Inst_add);
    insert_str("sub", order_tree, NULL, Inst_sub);
    insert_str("not", order_tree, NULL, Inst_not);
    insert_str("clr", order_tree, NULL, Inst_clr);
    insert_str("lea", order_tree, NULL, Inst_lea);
    insert_str("inc", order_tree, NULL, Inst_inc);
    insert_str("dec", order_tree, NULL, Inst_dec);
    insert_str("jmp", order_tree, NULL, Inst_jmp);
    insert_str("bne", order_tree, NULL, Inst_bne);
    insert_str("red", order_tree, NULL, Inst_red);
    insert_str("prn", order_tree, NULL, Inst_prn);
    insert_str("jsr", order_tree, NULL, Inst_jsr);
    insert_str("rts", order_tree, NULL, Inst_rts);
    insert_str("stop", order_tree, NULL, Inst_stop);

    insert_str("data", order_tree, NULL, dir_data);
    insert_str("string", order_tree, NULL, dir_string);
    insert_str("entry", order_tree, NULL, dir_entry);
    insert_str("extern", order_tree, NULL, dir_extern);
}