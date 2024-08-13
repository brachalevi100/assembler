#ifndef TREE_H
#define TREE_H

#define TUB_BEGINING 32
#define NUM_TREE_PTR 95

/**
 * Enumeration representing the possible states of a string in a tree node.
 */
enum sut {
    not_end, /**< The string is not at its end in the tree. */
    end     /**< The string has reached its end in the tree. */
};

/**
 * A structure representing a node in the tree.
 */
typedef struct tree_node {
    char current_char;          /**< The current character associated with the node. */
    int type;                   /**< The type associated with the string that ends at this node. */
    int end_str;                /**< Represents whether the string ends at this node or not (enum sut). */
    void* get_back;             /**< A pointer to associated data linked with the string ending at this node. */
    struct tree_node* next[NUM_TREE_PTR]; /**< An array of pointers to the next nodes in the tree. */
} Tree_node;

/**
 * A structure representing the root of the tree.
 */
typedef struct tree {
    Tree_node* next[NUM_TREE_PTR]; /**< An array of pointers to the next nodes in the tree from the root. */
} Tree;

/**
 * Checks if a given string exists in the provided tree and returns a pointer to the associated data.
 *
 * @param str The string to search for in the tree.
 * @param tree The tree structure containing the data.
 * @return A pointer to the associated data if the string exists, otherwise NULL.
 */
void* str_exist(const char* str, Tree* tree);

/**
 * Checks if a given string exists in the provided tree and returns its associated type.
 *
 * @param str The string to search for in the tree.
 * @param tree The tree structure containing the data.
 * @return The associated type of the string if it exists, otherwise -1.
 */
int str_exist_type(const char* str, Tree* tree);

/**
 * Deletes a string from the provided tree, marking it as not ending and removing its associated data.
 *
 * @param tree The tree structure containing the data.
 * @param str The string to be deleted from the tree.
 */
void delete_str(Tree* tree, const char* str);

/**
 * Frees the memory occupied by the tree structure and its associated data.
 *
 * @param tree The tree structure to be freed.
 */
void free_tree(Tree* tree);

/**
 * Inserts a string into the tree along with associated data and type.
 *
 * @param str The string to be inserted into the tree.
 * @param tree The tree structure to insert the string into.
 * @param get_back A pointer to the associated data to be stored with the string.
 * @param type The type of the string to be stored.
 */
void insert_str(const char* str, Tree* tree, void* get_back, int type);

#endif
