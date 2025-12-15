#include "avl.h"

// Node creation
AVLNode* avl_create_node(void *data) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = data;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Size of an AVL 
int avl_height(AVLNode *node) {
    if (node == NULL) return 0;
    return node->height;
}

// Maximum of two integers
int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

// Right rotation
AVLNode* avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    return x;
}

// Right rotation
AVLNode* avl_rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    return y;
}

// Get balance factor
int avl_get_balance(AVLNode *node) {
    if (node == NULL) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

// Insert a node
AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*)) {
    if (root == NULL) return avl_create_node(data);

    int cmp = compare(data, root->data);
    if (cmp > 0) {
        root->left = avl_insert(root->left, data, compare);
    } else if (cmp < 0) {
        root->right = avl_insert(root->right, data, compare);
    } else {
        return root;
    }

    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));
    int balance = avl_get_balance(root);

    if (balance > 1 && compare(data, root->left->data) > 0) {
        return avl_rotate_right(root);
    }
    if (balance < -1 && compare(data, root->right->data) < 0) {
        return avl_rotate_left(root);
    }
    if (balance > 1 && compare(data, root->left->data) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }
    if (balance < -1 && compare(data, root->right->data) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }
    return root;
}

// Search a node by id
void* avl_search(AVLNode *root, const char *id, int (*compare)(const void*, const char*)) {
    if (root == NULL) return NULL;

    int cmp = compare(root->data, id);
    if (cmp == 0) {
        return root->data;
    } else if (cmp > 0) {
        return avl_search(root->left, id, compare);
    } else {
        return avl_search(root->right, id, compare);
    }
}

// Inorder traversal
void avl_inorder(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*)) {
    if (root == NULL) return;
    avl_inorder(root->left, file, print_data);
    print_data(root->data, file);
    avl_inorder(root->right, file, print_data);
}

// Reverse inorder traversal
void avl_inorder_reverse(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*)) {
    if (root == NULL) return;
    avl_inorder_reverse(root->right, file, print_data);
    print_data(root->data, file);
    avl_inorder_reverse(root->left, file, print_data);
}

// Free the AVL tree
void avl_free(AVLNode *root, void (*free_data)(void*)) {
    if (root == NULL) return;
    avl_free(root->left, free_data);
    avl_free(root->right, free_data);
    free_data(root->data);
    free(root);
}

// Compare factories
int compare_factories(const void *a, const void *b) {
    Factory *fa = (Factory*)a;
    Factory *fb = (Factory*)b;
    return strcmp(fa->id, fb->id);
}

// Search factory by id
int search_factory(const void *a, const char *id) {
    Factory *f = (Factory*)a;
    return strcmp(f->id, id);
}

// Compare network nodes
int compare_network_nodes(const void *a, const void *b) {
    NetworkNode *na = (NetworkNode*)a;
    NetworkNode *nb = (NetworkNode*)b;
    return strcmp(na->id, nb->id);
}

// Search network node by id
int search_network_node(const void *a, const char *id) {
    NetworkNode *n = (NetworkNode*)a;
    return strcmp(n->id, id);
}

// Display and release functions
void print_factory(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    fprintf(file, "%s;%.3f;%.3f;%.3f\n", f->id, f->max_volume / 1000.0, f->source_volume / 1000.0, f->real_volume / 1000.0);
}

// Free factory
void free_factory(void *data) {
    Factory *f = (Factory*)data;
    free(f->id);
    free(f);
}

// Print network node
void print_network_node(void *data, FILE *file) {
    NetworkNode *n = (NetworkNode*)data;
    fprintf(file, "%s\n", n->id);
}

// Free network node
void free_network_node(void *data) {
    NetworkNode *n = (NetworkNode*)data;
    free(n->id);
    free(n);
}
