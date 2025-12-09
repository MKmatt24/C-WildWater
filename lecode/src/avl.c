#include "avl.h"

// Create a new generic AVL node
AVLNode* avl_create_node(void *data) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = data;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Get the height of a node
int avl_height(AVLNode *node) {
    if (node == NULL) return 0;
    return node->height;
}

// Get the maximum of two integers
int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

// Perform a right rotation
AVLNode* avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    return x;
}

// Perform a left rotation
AVLNode* avl_rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    return y;
}

// Get the balance factor of a node
int avl_get_balance(AVLNode *node) {
    if (node == NULL) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

// Generic insertion in AVL tree
AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*)) {
    if (root == NULL) {
        return avl_create_node(data);
    }
    int cmp = compare(data, root->data);
    if (cmp > 0) { // Sort by reverse alphabetical order for factories
        root->left = avl_insert(root->left, data, compare);
    } else if (cmp < 0) {
        root->right = avl_insert(root->right, data, compare);
    } else {
        // Update data if the node already exists
        if (compare == compare_usines) {
            Usine *existing = (Usine*)root->data;
            Usine *new = (Usine*)data;
            existing->max_volume = new->max_volume;
            existing->source_volume = new->source_volume;
            existing->real_volume = new->real_volume;
        }
        free(data);
        return root;
    }
    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));
    int balance = avl_get_balance(root);
    // Left Left case
    if (balance > 1 && compare(data, root->left->data) > 0) {
        return avl_rotate_right(root);
    }
    // Right Right case
    if (balance < -1 && compare(data, root->right->data) < 0) {
        return avl_rotate_left(root);
    }
    // Left Right case
    if (balance > 1 && compare(data, root->left->data) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }
    // Right Left case
    if (balance < -1 && compare(data, root->right->data) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }
    return root;
}
