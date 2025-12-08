#include "avl.h"

// Créer un nouveau nœud AVL générique
AVLNode* avl_create_node(void *data) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    node->data = data;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Hauteur d'un nœud
int avl_height(AVLNode *node) {
    if (node == NULL) return 0;
    return node->height;
}

// Maximum de deux entiers
int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

// Rotation droite
AVLNode* avl_rotate_right(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;

    return x;
}

// Rotation gauche
AVLNode* avl_rotate_left(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;

    return y;
}

// Équilibrage d'un nœud
int avl_get_balance(AVLNode *node) {
    if (node == NULL) return 0;
    return avl_height(node->left) - avl_height(node->right);
}

// Insertion générique dans l'AVL
AVLNode* avl_insert(AVLNode *root, void *data, int (*compare)(const void*, const void*)) {
    if (root == NULL) {
        return avl_create_node(data);
    }

    int cmp = compare(data, root->data);

    if (cmp > 0) { // Tri par ordre alphabétique inverse pour les usines
        root->left = avl_insert(root->left, data, compare);
    } else if (cmp < 0) {
        root->right = avl_insert(root->right, data, compare);
    } else {
        // Mise à jour des données si le nœud existe déjà
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

    // Cas Left Left
    if (balance > 1 && compare(data, root->left->data) > 0) {
        return avl_rotate_right(root);
    }

    // Cas Right Right
    if (balance < -1 && compare(data, root->right->data) < 0) {
        return avl_rotate_left(root);
    }

    // Cas Left Right
    if (balance > 1 && compare(data, root->left->data) < 0) {
        root->left = avl_rotate_left(root->left);
        return avl_rotate_right(root);
    }

    // Cas Right Left
    if (balance < -1 && compare(data, root->right->data) > 0) {
        root->right = avl_rotate_right(root->right);
        return avl_rotate_left(root);
    }

    return root;
}

// Recherche générique dans l'AVL
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

// Parcours infixe générique pour générer un fichier
void avl_inorder(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*)) {
    if (root == NULL) return;

    avl_inorder(root->left, file, print_data);
    print_data(root->data, file);
    avl_inorder(root->right, file, print_data);
}

// Libération générique de la mémoire
void avl_free(AVLNode *root, void (*free_data)(void*)) {
    if (root == NULL) return;

    avl_free(root->left, free_data);
    avl_free(root->right, free_data);

    free_data(root->data);
    free(root);
}

// Fonctions de comparaison pour les usines
int compare_usines(const void *a, const void *b) {
    Usine *ua = (Usine*)a;
    Usine *ub = (Usine*)b;
    return strcmp(ub->id, ua->id); // Tri par ordre alphabétique inverse
}

int search_usine(const void *a, const char *id) {
    Usine *u = (Usine*)a;
    return strcmp(u->id, id);
}

// Fonctions de comparaison pour les nœuds
int compare_noeuds(const void *a, const void *b) {
    Noeud *na = (Noeud*)a;
    Noeud *nb = (Noeud*)b;
    return strcmp(na->id, nb->id);
}

int search_noeud(const void *a, const char *id) {
    Noeud *n = (Noeud*)a;
    return strcmp(n->id, id);
}

// Fonctions d'affichage pour les usines
void print_usine(void *data, FILE *file) {
    Usine *u = (Usine*)data;
    fprintf(file, "%s;%.3f;%.3f;%.3f\n", u->id, u->max_volume, u->source_volume, u->real_volume);
}

// Fonctions de libération pour les usines
void free_usine(void *data) {
    Usine *u = (Usine*)data;
    free(u->id);
    free(u);
}

// Fonctions d'affichage pour les nœuds
void print_noeud(void *data, FILE *file) {
    Noeud *n = (Noeud*)data;
    fprintf(file, "%s\n", n->id);
}

// Fonctions de libération pour les nœuds
void free_noeud(void *data) {
    Noeud *n = (Noeud*)data;
    free(n->id);
    free(n);
}
