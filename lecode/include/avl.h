#ifndef AVL_H
#define AVL_H

#include <stdio.h>

// AVL Structure
typedef struct AVLNode {
    void *data;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

// Fonctions de base de l'AVL
AVLNode* avl_create_node(void *data);
int avl_height(AVLNode *node);
int avl_max(int a, int b);
AVLNode* avl_rotate_right(AVLNode *y);
AVLNode* avl_rotate_left(AVLNode *x);
int avl_get_balance(AVLNode *node);
AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*));
void* avl_search(AVLNode *root, const char *id, int (*compare)(const void*, const char*));
void avl_inorder_reverse(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*));
void avl_free(AVLNode *root, void (*free_data)(void*));

#endif
