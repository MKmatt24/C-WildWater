#ifndef AVL_H
#define AVL_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Factory {
    char *id;
    double max_volume;
    double source_volume;
    double real_volume;
} Factory;

typedef struct NetworkNode {
    char *id;
    struct NetworkNode *parent;
} NetworkNode;

typedef struct AVLNode {
    void *data;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

// Prototypes des fonctions
AVLNode* avl_create_node(void *data);
AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*));
void* avl_search(AVLNode *root, const char *id, int (*compare)(const void*, const char*));
void avl_inorder(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*));
void avl_inorder_reverse(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*));
void avl_free(AVLNode *root, void (*free_data)(void*));

// Fonctions de comparaison
int compare_factories(const void *a, const void *b);
int search_factory(const void *a, const char *id);
int compare_network_nodes(const void *a, const void *b);
int search_network_node(const void *a, const char *id);

// Fonctions d'affichage et de libération
void print_factory(void *data, FILE *file);
void free_factory(void *data);
void print_network_node(void *data, FILE *file);
void free_network_node(void *data);

#endif
