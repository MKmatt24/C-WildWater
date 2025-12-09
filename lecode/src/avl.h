#ifndef AVL_H
#define AVL_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Generic structure for an AVL node
typedef struct AVLNode {
    void *data;                     // Pointer to data (Factory or Node)
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

// Structure for a factory
typedef struct Usine {
    char *id;
    double max_volume;      // Maximum processing capacity (in thousands of m³/year)
    double source_volume;   // Total volume captured by sources (in thousands of m³/year)
    double real_volume;     // Actual processed volume (in thousands of m³/year)
} Usine;

// Structure for a network node
typedef struct Noeud {
    char *id;
    // Other fields depending on the node type (source, storage, junction, etc.)
} Noeud;

// Functions for the generic AVL
AVLNode* avl_create_node(void *data);
AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*));
void* avl_search(AVLNode *root, const char *id, int (*compare)(const void*, const char*));
void avl_inorder(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*));
void avl_free(AVLNode *root, void (*free_data)(void*));

// Comparison functions for AVL
int compare_usines(const void *a, const void *b);
int search_usine(const void *a, const char *id);
int compare_noeuds(const void *a, const void *b);
int search_noeud(const void *a, const char *id);

// Display and free functions for AVL
void print_usine(void *data, FILE *file);
void free_usine(void *data);
void print_noeud(void *data, FILE *file);
void free_noeud(void *data);
#endif
