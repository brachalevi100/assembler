#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

void* str_exist(const char* str, Tree* tree) {
    if (tree->next[*str - TUB_BEGINING] == NULL)
        return NULL;
    Tree_node* tree_node = (tree->next[*str - TUB_BEGINING]);
    str++;
    while (*str != '\0') {
        if (tree_node->next[*str - TUB_BEGINING] == NULL)
            return NULL;
        tree_node = tree_node->next[*str - TUB_BEGINING];
        str++;
    }
    if (tree_node->end_str == end) {
        return tree_node->get_back;
    }
    else return NULL;
}

int str_exist_type(const char* str, Tree* tree) {
    Tree_node* tree_node;
    if (tree->next[*str - TUB_BEGINING] == NULL)
        return -1;
    tree_node = (tree->next[*str - TUB_BEGINING]);
    str++;
    while (*str != '\0') {
        if (tree_node->next[*str - TUB_BEGINING] == NULL)
            return -1;
        tree_node = tree_node->next[*str - TUB_BEGINING];
        str++;
    }
    if (tree_node->end_str == end) {
        return tree_node->type;
    }
    else return -1;
}


static void free_next(Tree_node* tree_node) {
    if (tree_node == NULL)
        return;
    for (int i = 0; i < NUM_TREE_PTR; i++)
        free_next(tree_node->next[i]);
    free(tree_node);
}

void free_tree(Tree* tree) {
    for (int i = 0; i < NUM_TREE_PTR; i++)
        free_next(tree->next[i]);
    free(tree);
}


void insert_str(const char* str, Tree* tree, void* get_back, int type) {
    Tree_node* tree_node;
    if (tree->next[*str - TUB_BEGINING] == NULL)
        tree->next[*str - TUB_BEGINING] = calloc(1, sizeof(Tree_node));
    tree_node = (tree->next[*str - TUB_BEGINING]);
    str++;
    while (*str != '\0') {
        if (tree_node->next[*str - TUB_BEGINING] == NULL)
            tree_node->next[*str - TUB_BEGINING] = calloc(1, sizeof(Tree_node));
        tree_node = tree_node->next[*str - TUB_BEGINING];
        str++;
    }
    tree_node->end_str = end;
    tree_node->type = type;
    tree_node->get_back = get_back;
}