#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "avl.h"

//Basic AVL functions 
AVLNode* avl_create_node(void *data) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) return NULL;
    node->data = data;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

int avl_height(AVLNode *node) {
    return node ? node->height : 0;
}

int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

AVLNode* avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    return x;
}

AVLNode* avl_rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    return y;
}

int avl_get_balance(AVLNode *node) {
    return node ? avl_height(node->left) - avl_height(node->right) : 0;
}

AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*)) {
    if (root == NULL) return avl_create_node(data);

    int cmp = compare(data, root->data);
    if (cmp < 0) {
        root->left = avl_insert(root->left, data, compare);
    } else if (cmp > 0) {
        root->right = avl_insert(root->right, data, compare);
    } else {
        return root;
    }

    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));
    int balance = avl_get_balance(root);

    if (balance > 1 && compare(data, root->left->data) < 0) {
        return avl_rotate_right(root);
    }
    if (balance < -1 && compare(data, root->right->data) > 0) {
        return avl_rotate_left(root);
    }
    if (balance > 1 && compare(data, root->left->data) > 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }
    if (balance < -1 && compare(data, root->right->data) < 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }
    return root;
}

void* avl_search(AVLNode *root, const char *id, int (*compare)(const void*, const char*)) {
    if (root == NULL) return NULL;
    int cmp = compare(root->data, id);
    if (cmp == 0) return root->data;
    else if (cmp > 0) return avl_search(root->left, id, compare);
    else return avl_search(root->right, id, compare);
}

void avl_inorder_reverse(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*)) {
    if (root == NULL) return;
    avl_inorder_reverse(root->right, file, print_data);
    print_data(root->data, file);
    avl_inorder_reverse(root->left, file, print_data);
}

void avl_free(AVLNode *root, void (*free_data)(void*)) {
    if (root == NULL) return;
    avl_free(root->left, free_data);
    avl_free(root->right, free_data);
    free_data(root->data);
    free(root);
}
