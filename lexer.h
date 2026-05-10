#ifndef LEXER_H
#define LEXER_H

#include "function.h"

/**
 * @enum DuplicateChecker
 * Enumerates the duplicate checker options.
 */
enum DuplicateChecker {
	on,             /**< Enable duplicate checking */
	off,            /**< Disable duplicate checking */
	duplicateChecker /**< Enable duplicate checking with extra details */
};

/**
 * @enum Operand_option
 * Enumerates the different operand options for instructions.
 */
enum Operand_option {
	operand_non = 0,         /**< No operand */
	operand_const_num = 1,   /**< Constant number operand */
	operand_lable = 3,       /**< Label operand */
	operand_register = 5     /**< Register operand */
};

/**
 * @union Operand
 * Represents the possible operands for instructions.
 */
union Operand {
	int const_num;      /**< Constant number operand */
	int register_num;   /**< Register operand */
	char lable[MAX_LABLE_LEN]; /**< Label operand */
};

/**
 * @struct Inst
 * Represents an instruction with its operand details.
 */
struct Inst {
	int inst_type;                /**< Type of instruction */
	enum Operand_option operand_options[2]; /**< Operand options */
	union Operand operand[2];      /**< Operands */
};

/**
 * @union Dir_operand
 * Represents the possible operands for directives.
 */
union Dir_operand {
	char* lable_name[40]; /**< Label name operand */
	char* string;         /**< String operand */
	int data[40];         /**< Data operand */
};

/**
 * @struct Dir
 * Represents a directive with its operand details.
 */
struct Dir {
	int count_par;      /**< Number of parameters */
	Dir_opt dir_type; /**< Type of directive */
	union Dir_operand dir_operand; /**< Operand details */
};

/**
 * @union Details
 * Represents the possible details of an AST node.
 */
union Details {
	struct Dir dir;     /**< Directive details */
	struct Inst inst;   /**< Instruction details */
};

/**
 * @struct AST
 * Represents an Abstract Syntax Tree (AST) node.
 */
typedef struct {
	char* lable_name;   /**< Name of the label */
	char error[MAX_ERROR_LEN];    /**< Error message */

	int row_type;       /**< Type of row */
	union Details details; /**< Details of the row */
} AST;

/**
 * Creates an error AST node.
 * @param ast The AST node to mark as an error.
 * @return A pointer to the error AST node.
 */
AST* error_ast(AST* ast);

/**
 * Analyzes a line of code and constructs an AST node.
 *
 * @param order_tree The tree containing language order.
 * @param line The line of code to analyze.
 * @return A pointer to the constructed AST node on success, or NULL on error.
 */
AST* lexer(Tree* order_tree, char* line);

#endif
