#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"

bool check_factory_line(char** cols, int num_cols) {
    if (num_cols < 5) return false;
    return (strcmp(cols[0], "-") == 0 && 
            strcmp(cols[2], "-") == 0 && 
            strcmp(cols[3], "-") != 0 &&
            strcmp(cols[4], "-") == 0);
}

bool check_source_line(char** cols, int num_cols) {
    if (num_cols < 5) return false;
    return (strcmp(cols[0], "-") == 0 && 
            strcmp(cols[2], "-") != 0 && 
            strcmp(cols[4], "-") != 0);
}

int compare_factories(const void *a, const void *b) {
    return strcmp(((Factory*)a)->id, ((Factory*)b)->id);
}

int search_factory(const void *a, const char *id) {
    return strcmp(((Factory*)a)->id, id);
}

void free_factory(void *data) {
    Factory *f = (Factory*)data;
    free(f->id);
    free(f);
}

void print_factory_max(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    if (f->max_volume > 0.001) {
        fprintf(file, "%s;%.3f\n", f->id, f->max_volume);
    }
}

void print_factory_src(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    if (f->source_volume > 0.001) {
        fprintf(file, "%s;%.3f\n", f->id, f->source_volume);
    }
}

void print_factory_real(void *data, FILE *file) {
    Factory *f = (Factory*)data;
    if (f->real_volume > 0.001) {
        fprintf(file, "%s;%.3f\n", f->id, f->real_volume);
    }
}

int compare_node_lookup(const void *a, const void *b) {
    return strcmp(((NodeLookup*)a)->id, ((NodeLookup*)b)->id);
}

int search_node_lookup(const void *a, const char *id) {
    return strcmp(((NodeLookup*)a)->id, id);
}

void free_lookup(void *data) {
    NodeLookup *nl = (NodeLookup*)data;
    free(nl->id);
    free(nl);
}

double compute_network_leaks(NetworkNode *node, double entering_volume) {
    if (node == NULL || node->num_children == 0) return 0.0;

    double total_leaks = 0.0;
    double volume_per_child = entering_volume / node->num_children;

    ChildNode *current = node->children;
    while (current != NULL) {
        double edge_leak = volume_per_child * (current->leak_percentage / 100.0);
        total_leaks += edge_leak;
        double remaining_vol = volume_per_child - edge_leak;
        total_leaks += compute_network_leaks(current->node, remaining_vol);
        current = current->next;
    }
    return total_leaks;
}

void free_network(NetworkNode *node) {
    if (!node) return;
    ChildNode *curr = node->children;
    while (curr) {
        ChildNode *tmp = curr;
        curr = curr->next;
        free_network(tmp->node);
        free(tmp);
    }
    free(node->id);
    free(node);
}

AVLNode* build_avl(const char *filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        return NULL;
    }

    AVLNode* root = NULL;
    char line[MAX_LINE];
    int line_num = 0;
    int factory_count = 0;
    int source_count = 0;

    while (fgets(line, sizeof(line), f)) {
        line_num++;
        
        if (line_num == 1 && strstr(line, "Factory_ID") != NULL) {
            continue;
        }

        char* cols[5];
        char* tmp = strtok(line, ";\n");
        int i = 0;

        while (tmp && i < 5) {
            cols[i++] = tmp;
            tmp = strtok(NULL, ";\n");
        }
        
        if (i < 4) continue;
        
        Factory* p = NULL;

        if (check_factory_line(cols, i)) {
            factory_count++;
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
        else if (check_source_line(cols, i)) {
            source_count++;
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
    
    fprintf(stderr, "DEBUG: Lines processed: %d\n", line_num);
    fprintf(stderr, "DEBUG: Factory lines found: %d\n", factory_count);
    fprintf(stderr, "DEBUG: Source lines found: %d\n", source_count);
    
    return root;
}
