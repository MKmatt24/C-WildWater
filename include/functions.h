#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdbool.h>
#include "avl.h"

#define MAX_LINE 2048


typedef struct Factory {
    char *id;
    double max_volume;
    double source_volume;
    double real_volume;
} Factory;


typedef struct ChildNode {
    struct NetworkNode *node;
    double leak_percentage;
    struct ChildNode *next;
} ChildNode;


typedef struct NetworkNode {
    char *id;
    ChildNode *children;
    int num_children;
} NetworkNode;


typedef struct NodeLookup {
    char *id;
    NetworkNode *node_ptr;
} NodeLookup;

// Bool verifications for CSV
bool check_factory_line(char** cols, int num_cols);
bool check_source_line(char** cols, int num_cols);

// Comparaison functions.
int compare_factories(const void *a, const void *b);
int search_factory(const void *a, const char *id);
void free_factory(void *data);

void print_factory_max(void *data, FILE *file);
void print_factory_src(void *data, FILE *file);
void print_factory_real(void *data, FILE *file);

// Research functions
int compare_node_lookup(const void *a, const void *b);
int search_node_lookup(const void *a, const char *id);
void free_lookup(void *data);

// distribution network functions
double compute_network_leaks(NetworkNode *node, double entering_volume);
void free_network(NetworkNode *node);

// AVL constructions thing
AVLNode* build_avl(const char *filename);



#endif
