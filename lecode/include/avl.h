#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

/* Network structures */

struct NetworkNode;

typedef struct NetworkNodeList {
    struct NetworkNode *node;
    struct NetworkNodeList *next;
} NetworkNodeList;

typedef struct NetworkNode {
    char *id;
    double leak_percent;
    struct NetworkNode *parent;
    NetworkNodeList *children;
} NetworkNode;

/* Factory structure */

typedef struct Factory {
    char *id;
    double max_volume;
    double source_volume;
    double real_volume;
    NetworkNode *network_root;
    struct AVLNode *network_index;
} Factory;

/* AVL structure */

typedef struct AVLNode {
    void *data;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

/* AVL API */

AVLNode *avl_create_node(void *data);
AVLNode *avl_insert(AVLNode *root, void *data, int (*compare)(const void *, const void *));
void *avl_search(AVLNode *root, const void *key, int (*compare)(const void *, const void *));
void avl_inorder(AVLNode *root, FILE *out, void (*print_data)(void *, FILE *));
void avl_inorder_reverse(AVLNode *root, FILE *out, void (*print_data)(void *, FILE *));
void avl_free(AVLNode *root, void (*free_data)(void *));

/* Factory helpers */

Factory *avl_insert_factory(AVLNode **factory_avl, Factory *factory);
int compare_factories(const void *a, const void *b);
int compare_factory_id(const void *key, const void *data);
void free_factory(void *data);


/* Network helpers */

NetworkNode *create_network_node(const char *id, double leak_percent);
NetworkNode *avl_insert_network_node(Factory *factory, NetworkNode *node);
int compare_network_nodes(const void *a, const void *b);
int compare_network_node_id(const void *key, const void *data);
void free_network_tree(NetworkNode *root);






#endif
