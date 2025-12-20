#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdbool.h>
#include "avl.h"

#define MAX_LINE 2048

// Structure Factory pour les usines
typedef struct Factory {
    char *id;
    double max_volume;
    double source_volume;
    double real_volume;
} Factory;

// Structure pour la liste chaînée des nœuds enfants
typedef struct ChildNode {
    struct NetworkNode *node;
    double leak_percentage;
    struct ChildNode *next;
} ChildNode;

// Structure du nœud du réseau (arbre généraliste)
typedef struct NetworkNode {
    char *id;
    ChildNode *children;
    int num_children;
} NetworkNode;

// Structure pour l'AVL de recherche (stocke l'adresse des NetworkNode par ID)
typedef struct NodeLookup {
    char *id;
    NetworkNode *node_ptr;
} NodeLookup;

// Fonctions de vérification des lignes CSV
bool check_factory_line(char** cols);
bool check_source_line(char** cols);

// Fonctions de comparaison pour Factory
int compare_factories(const void *a, const void *b);
int search_factory(const void *a, const char *id);
void free_factory(void *data);

// Fonctions d'affichage pour Factory
void print_factory_max(void *data, FILE *file);
void print_factory_src(void *data, FILE *file);
void print_factory_real(void *data, FILE *file);

// Fonctions de comparaison pour NodeLookup
int compare_node_lookup(const void *a, const void *b);
int search_node_lookup(const void *a, const char *id);
void free_lookup(void *data);

// Fonctions pour le réseau de distribution
double compute_network_leaks(NetworkNode *node, double entering_volume);
void free_network(NetworkNode *node);

// Fonction principale de construction de l'AVL
AVLNode *build_avl(const char *filename);

#endif
