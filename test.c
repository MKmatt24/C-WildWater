#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 2048

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

// Vérifie s'il s'agit d'une ligne décrivant une usine
bool check_factory_line(char** cols) {
    return (strcmp(cols[0], "-") == 0 && 
            strcmp(cols[2], "-") == 0 && 
            strcmp(cols[3], "-") != 0);
}

// Vérifie s'il s'agit d'une ligne de captage SOURCE -> USINE
bool check_source_line(char** cols) {
    return (strcmp(cols[0], "-") == 0 && 
            strcmp(cols[2], "-") != 0 && 
            strcmp(cols[4], "-") != 0);
}

// Node creation
AVLNode* avl_create_node(void *data) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) return NULL;
    node->data = data;
    node->left = node->right = NULL;
    node->height = 1;
    return node;
}

// Height of an AVL node
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

// Left rotation
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

// Reverse inorder traversal
void avl_inorder_reverse(AVLNode *root, FILE *file, void (*print_data)(void*, FILE*)) {
    if (root == NULL) {
        return;
    }    
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

// Free factory
void free_factory(void *data) {
    Factory *f = (Factory*)data;
    free(f->id);
    free(f);
}

// Affichage pour l'argument 'max'
void print_factory_max(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    fprintf(file, "%s;%.3f\n", f->id, f->max_volume / 1000.0);
}

// Affichage pour l'argument 'src'
void print_factory_src(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    fprintf(file, "%s;%.3f\n", f->id, f->source_volume / 1000.0);
}

// Affichage pour l'argument 'real'
void print_factory_real(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    fprintf(file, "%s;%.3f\n", f->id, f->real_volume / 1000.0);
}

AVLNode *build_avl(const char *filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return NULL;
    }

    AVLNode* root = NULL;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), f)) {
        char* cols[5];
        char* tmp = strtok(line, ";\n");
        int i = 0;

        // Découpage en 5 colonnes
        while (tmp && i < 5) {
            cols[i++] = tmp;
            tmp = strtok(NULL, ";\n");
        }
        
        if (i < 4) {
            continue; // Ligne incomplète
        }        
        
        Factory* p = NULL;

        // CAS 1 : Ligne décrivant la capacité de l'USINE
        if (check_factory_line(cols)) {
            p = (Factory*)avl_search(root, cols[1], search_factory);
            if (!p) {
                p = calloc(1, sizeof(Factory));
                if (!p) {
                    fclose(f);
                    avl_free(root, free_factory);
                    return NULL;
                }
                p->id = strdup(cols[1]);
                if (!p->id) {
                    free(p);
                    fclose(f);
                    avl_free(root, free_factory);
                    return NULL;
                }
                root = avl_insert(root, p, compare_factories);
            }
            p->max_volume = atof(cols[3]);
        }

        // CAS 2 : Ligne de captage SOURCE -> USINE
        else if (check_source_line(cols)) {
            p = (Factory*)avl_search(root, cols[2], search_factory);
            if (!p) {
                p = calloc(1, sizeof(Factory));
                if (!p) {
                    fclose(f);
                    avl_free(root, free_factory);
                    return NULL;
                }
                p->id = strdup(cols[2]);
                if (!p->id) {
                    free(p);
                    fclose(f);
                    avl_free(root, free_factory);
                    return NULL;
                }
                root = avl_insert(root, p, compare_factories);
            }

            double volume_capte = atof(cols[3]);
            double fuite_ratio = atof(cols[4]) / 100.0;

            p->source_volume += volume_capte;
            p->real_volume += volume_capte * (1.0 - fuite_ratio);
        }
    }

    fclose(f);
    return root;
}

int main(int argc, char *argv[]) {
    // Vérification minimale des arguments
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <input.csv> <mode> <output_file> [...]\n", argv[0]);
        return 1;
    }

    // Construction de l'arbre AVL
    AVLNode* root = build_avl(argv[1]);
    if (!root) {
        fprintf(stderr, "Error: Failed to build AVL from %s\n", argv[1]);
        return 2;
    }

    // Parcours des arguments pour traiter les différents modes
    for (int i = 2; i < argc; i++) {
        
        // --- Cas MAX ---
        if (strcmp(argv[i], "max") == 0 && (i + 1) < argc) {
            FILE* out = fopen(argv[i+1], "w");
            if (out) {
                fprintf(out, "identifier;max volume (M.m3)\n");
                avl_inorder_reverse(root, out, print_factory_max);
                fclose(out);
            } else {
                fprintf(stderr, "Error: cannot open output file %s\n", argv[i+1]);
            }
            i++;
        } 
        
        // --- Cas SRC ---
        else if (strcmp(argv[i], "src") == 0 && (i + 1) < argc) {
            FILE* out = fopen(argv[i+1], "w");
            if (out) {
                fprintf(out, "identifier;source volume (M.m3)\n");
                avl_inorder_reverse(root, out, print_factory_src);
                fclose(out);
            } else {
                fprintf(stderr, "Error: cannot open output file %s\n", argv[i+1]);
            }
            i++;
        }
        
        // --- Cas REAL ---
        else if (strcmp(argv[i], "real") == 0 && (i + 1) < argc) {
            FILE* out = fopen(argv[i+1], "w");
            if (out) {
                fprintf(out, "identifier;real volume (M.m3)\n");
                avl_inorder_reverse(root, out, print_factory_real);
                fclose(out);
            } else {
                fprintf(stderr, "Error: cannot open output file %s\n", argv[i+1]);
            }
            i++;
        }
    }

    // Nettoyage final de la mémoire
    avl_free(root, free_factory);
    
    return 0;
}
