#include "avl.h"
#include <string.h>

/* Creates a new AVL node */
AVLNode *avl_create_node(void *data) {
    AVLNode *node = malloc(sizeof(AVLNode));
    if (!node) {
        return NULL;
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

/* Returns the height of a node */
static int height(AVLNode *node) {
    if (!node) {
        return 0;
    }
    return node->height;
}

/* Returns the maximum of two integers */
static int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

/* Computes the balance factor of a node */
static int balance_factor(AVLNode *node) {
    if (!node) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

/* Performs a right rotation */
static AVLNode *rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *t2 = x->right;

    x->right = y;
    y->left = t2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

/* Performs a left rotation */
static AVLNode *rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *t2 = y->left;

    y->left = x;
    x->right = t2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

/* Inserts a generic element into an AVL tree */
AVLNode *avl_insert(AVLNode *root, void *data, int (*compare)(const void *, const void *)) {
    if (!root) {
        return avl_create_node(data);
    }

    int cmp = compare(data, root->data);

    if (cmp < 0) {
        root->left = avl_insert(root->left, data, compare);
    } else if (cmp > 0) {
        root->right = avl_insert(root->right, data, compare);
    } else {
        return root;
    }

    root->height = 1 + max(height(root->left), height(root->right));

    int bf = balance_factor(root);

    if (bf > 1 && compare(data, root->left->data) < 0) {
        return rotate_right(root);
    }

    if (bf < -1 && compare(data, root->right->data) > 0) {
        return rotate_left(root);
    }

    if (bf > 1 && compare(data, root->left->data) > 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }

    if (bf < -1 && compare(data, root->right->data) < 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }

    return root;
}

/* Searches an element in an AVL tree */
void *avl_search(AVLNode *root, const void *key, int (*compare)(const void *, const void *)) {
    if (!root) {
        return NULL;
    }

    int cmp = compare(key, root->data);

    if (cmp == 0) {
        return root->data;
    }
    if (cmp < 0) {
        return avl_search(root->left, key, compare);
    }
    return avl_search(root->right, key, compare);
}

/* Traverses the AVL tree in ascending order */
void avl_inorder(AVLNode *root, FILE *out, void (*print_data)(void *, FILE *)) {
    if (!root) {
        return;
    }
    avl_inorder(root->left, out, print_data);
    print_data(root->data, out);
    avl_inorder(root->right, out, print_data);
}

/* Traverses the AVL tree in descending order */
void avl_inorder_reverse(AVLNode *root, FILE *out, void (*print_data)(void *, FILE *)) {
    if (!root) {
        return;
    }
    avl_inorder_reverse(root->right, out, print_data);
    print_data(root->data, out);
    avl_inorder_reverse(root->left, out, print_data);
}

/* Frees an AVL tree */
void avl_free(AVLNode *root, void (*free_data)(void *)) {
    if (!root) {
        return;
    }

    avl_free(root->left, free_data);
    avl_free(root->right, free_data);

    if (free_data) {
        free_data(root->data);
    }

    free(root);
}

/* Inserts a factory into the main AVL and avoids duplicates */
Factory *avl_insert_factory(AVLNode **factory_avl, Factory *factory) {
    Factory *existing = avl_search(*factory_avl, factory->id, compare_factory_id);

    if (existing) {
        return existing;
    }

    *factory_avl = avl_insert(*factory_avl, factory, compare_factories);
    return factory;
}

/* Compares two factories by identifier */
int compare_factories(const void *a, const void *b) {
    const Factory *fa = a;
    const Factory *fb = b;
    return strcmp(fa->id, fb->id);
}

/* Compares a factory identifier with a factory */
int compare_factory_id(const void *key, const void *data) {
    const char *id = key;
    const Factory *f = data;
    return strcmp(id, f->id);
}

/* Frees a factory including its network tree and index */
void free_factory(void *data) {
    Factory *factory = data;

    if (factory->network_index) {
        avl_free(factory->network_index, NULL);
    }

    if (factory->network_root) {
        free_network_tree(factory->network_root);
    }

    free(factory->id);
    free(factory);
}

/* Creates a new network node */
NetworkNode *create_network_node(const char *id, double leak_percent) {
    NetworkNode *node = malloc(sizeof(NetworkNode));
    if (!node) {
        return NULL;
    }

    node->id = strdup(id);
    node->leak_percent = leak_percent;
    node->parent = NULL;
    node->children = NULL;

    return node;
}

/* Inserts a network node into the factory network index AVL and avoids duplicates */
NetworkNode *avl_insert_network_node(Factory *factory, NetworkNode *node) {
    NetworkNode *existing = avl_search(factory->network_index, node->id, compare_network_node_id);

    if (existing) {
        return existing;
    }
    factory->network_index = avl_insert(factory->network_index, node, compare_network_nodes);
    return node;
}

/* Compares two network nodes by identifier */
int compare_network_nodes(const void *a, const void *b) {
    const NetworkNode *na = a;
    const NetworkNode *nb = b;
    return strcmp(na->id, nb->id);
}

/* Compares a network node identifier with a network node */
int compare_network_node_id(const void *key, const void *data) {
    const char *id = key;
    const NetworkNode *n = data;
    return strcmp(id, n->id);
}

/* Frees a network tree recursively */
void free_network_tree(NetworkNode *root) {
    if (!root) {
        return;
    }

    NetworkNodeList *child = root->children;
    while (child) {
        NetworkNodeList *next = child->next;
        free_network_tree(child->node);
        free(child);
        child = next;
    }

    free(root->id);
    free(root);
}




