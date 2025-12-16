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

bool check_factory_line(char** cols) {
    return (strcmp(cols[0], "-") == 0 && 
            strcmp(cols[2], "-") == 0 && 
            strcmp(cols[3], "-") != 0);
}

// Vérifie s'il s'agit d'une ligne de captage SOURCE -> USINE [cite: 69-77]
// Format : - ; ID_SOURCE ; ID_USINE ; VOLUME ; FUITE
bool check_source_line(char** cols) {
    return (strcmp(cols[0], "-") == 0 && 
            strcmp(cols[2], "-") != 0 && 
            strcmp(cols[4], "-") != 0);
}

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
    if (root == NULL) {
        return;
    } 
    avl_inorder(root->left, file, print_data);
    print_data(root->data, file);
    avl_inorder(root->right, file, print_data);
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



// Affichage pour l'argument 'max' [cite: 179]
void print_factory_max(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    // Conversion : données initiales en k.m3 -> division par 1000 pour M.m3
    fprintf(file, "%s;%.3f\n", f->id, f->max_volume / 1000.0);
}

// Affichage pour l'argument 'src' [cite: 179]
void print_factory_src(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    fprintf(file, "%s;%.3f\n", f->id, f->source_volume / 1000.0);
}

// Affichage pour l'argument 'real' [cite: 180]
void print_factory_real(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    fprintf(file, "%s;%.3f\n", f->id, f->real_volume / 1000.0);
}






AVLNode *build_avl(const char *filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Error the file do not open \n");
        exit(1); // Code retour strictement positif en cas d'erreur [cite: 213]
    }

    AVLNode* root = NULL;
    char line[MAX_LINE]; // Taille adaptée pour traiter les lignes du fichier de 500Mo [cite: 56]

    while (fgets(line, sizeof(line), f)) {
        char* cols[5];
        char* tmp = strtok(line, ";\n");
        int i = 0;

        // Découpage en 5 colonnes selon le format CSV spécifié [cite: 59]
        while (tmp && i < 5) {
            cols[i++] = tmp;
            tmp = strtok(NULL, ";\n");
        }
        
        if (i < 4) continue; // Ligne incomplète

        Factory* p = NULL;

        // CAS 1 : Ligne décrivant la capacité de l'USINE [cite: 78, 86]
        if (check_factory_line(cols)) {
            p = avl_search(root, cols[1], search_factory);
            if (!p) {
                p = calloc(1, sizeof(Factory));
                p->id = strdup(cols[1]);
                root = avl_insert(root, p, compare_factories); // L'AVL s'équilibre ici
            }
            p->max_volume = atof(cols[3]); // Unité : milliers de m3 [cite: 86]
        }

        // CAS 2 : Ligne de captage SOURCE -> USINE [cite: 69, 75]
        else if (check_source_line(cols)) {
            p = avl_search(root, cols[2], search_factory);
            if (!p) {
                p = calloc(1, sizeof(Factory));
                p->id = strdup(cols[2]);
                root = avl_insert(root, p, compare_factories);
            }

            double volume_capte = atof(cols[3]); // Volume en milliers de m3 [cite: 75]
            double fuite_ratio = atof(cols[4]) / 100.0; // Pourcentage de fuites [cite: 75]

            p->source_volume += volume_capte;
            // Calcul du volume réellement traité après fuites [cite: 151, 152]
            p->real_volume += volume_capte * (1.0 - fuite_ratio);
        }
    }
}

    fclose(f);
    return root;





int main(int argc, char *argv[]) {
    // 1. Vérification des arguments
    // 2. Construction de l'arbre (La base de données en RAM)
    AVLNode* root = build_avl(argv[1]);
    
    // 3. Ouverture du fichier de sortie
    
    
    // 4. Choix du traitement et écriture
    if (strcmp(argv[2], "max") == 0) {
        FILE* out = fopen(argv[5], "w");
        fprintf(out, "identifier;max volume (M.m3)\n"); // En-tête [cite: 181]
        avl_inorder_reverse(root, out, print_factory_max);
    } 
    if (strcmp(argv[3], "src") == 0) {
        FILE* out = fopen(argv[6], "w");
        fprintf(out, "identifier;source volume (M.m3)\n");
        avl_inorder_reverse(root, out, print_factory_src);
    }
    if (strcmp(argv[4], "src") == 0) {
        FILE* out = fopen(argv[7], "w");
        fprintf(out, "identifier;source volume (M.m3)\n");
        avl_inorder_reverse(root, out, print_factory_src);
    }
    
    // 5. Nettoyage [cite: 219, 262]
    avl_free(root, free_factory);
    fclose(out);
    return 0;
}







