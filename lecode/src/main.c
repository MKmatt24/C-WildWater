#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "avl.h"
#include "functions.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <input.csv> <mode> <output_file/factory_id> [...]\n", argv[0]);
        return 1;
    }

    AVLNode* root = build_avl(argv[1]);
    if (!root) {
        fprintf(stderr, "Error: Failed to build AVL from %s\n", argv[1]);
        return 2;
    }

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "max") == 0 && (i + 1) < argc) {
            FILE* out = fopen(argv[i+1], "w");
            if (!out) {
                fprintf(stderr, "Error: cannot open output file %s\n", argv[i+1]);
                avl_free(root, free_factory);
                return 3;
            }
            fprintf(out, "identifier;max volume (k.m3.year-1)\n");
            avl_inorder_reverse(root, out, print_factory_max);
            fclose(out);
            i++;
        } 
        else if (strcmp(argv[i], "src") == 0 && (i + 1) < argc) {
            FILE* out = fopen(argv[i+1], "w");
            if (!out) {
                fprintf(stderr, "Error: cannot open output file %s\n", argv[i+1]);
                avl_free(root, free_factory);
                return 3;
            }
            fprintf(out, "identifier;source volume (k.m3.year-1)\n");
            avl_inorder_reverse(root, out, print_factory_src);
            fclose(out);
            i++;
        }
        else if (strcmp(argv[i], "real") == 0 && (i + 1) < argc) {
            FILE* out = fopen(argv[i+1], "w");
            if (!out) {
                fprintf(stderr, "Error: cannot open output file %s\n", argv[i+1]);
                avl_free(root, free_factory);
                return 3;
            }
            fprintf(out, "identifier;real volume (k.m3.year-1)\n");
            avl_inorder_reverse(root, out, print_factory_real);
            fclose(out);
            i++;
        }
        else if (strcmp(argv[i], "leaks") == 0 && (i + 1) < argc) {
            char *target_factory = argv[i+1];
            Factory *f_data = (Factory*)avl_search(root, target_factory, search_factory);
            
            FILE* out = fopen("leaks.dat", "a");
            if (!out) {
                fprintf(stderr, "Error: cannot open leaks.dat\n");
                avl_free(root, free_factory);
                return 3;
            }
            
            if (!f_data) {
                fprintf(out, "%s;-1\n", target_factory);
                fclose(out);
            } else {
                fclose(out);
                
                AVLNode *lookup_tree = NULL;
                NetworkNode *net_root = calloc(1, sizeof(NetworkNode));
                if (!net_root) { i++; continue; }
                net_root->id = strdup(target_factory);
                if (!net_root->id) { free(net_root); i++; continue; }
                
                NodeLookup *nl = malloc(sizeof(NodeLookup));
                if (!nl) { free(net_root->id); free(net_root); i++; continue; }
                nl->id = strdup(target_factory);
                if (!nl->id) { free(nl); free(net_root->id); free(net_root); i++; continue; }
                nl->node_ptr = net_root;
                lookup_tree = avl_insert(lookup_tree, nl, compare_node_lookup);

                bool nodes_added = true;
                int max_iterations = 100;
                int iteration = 0;
                
                while (nodes_added && iteration < max_iterations) {
                    nodes_added = false;
                    iteration++;
                    
                    FILE *data_file = fopen(argv[1], "r");
                    if (!data_file) break;
                    
                    char line[MAX_LINE];
                    int line_num = 0;
                    
                    while (fgets(line, sizeof(line), data_file)) {
                        line_num++;
                        
                        if (line_num == 1 && strstr(line, "Factory_ID") != NULL) {
                            continue;
                        }
                        
                        char *c[5];
                        char *t = strtok(line, ";\n");
                        int j = 0;
                        while (t && j < 5) {
                            c[j++] = t;
                            t = strtok(NULL, ";\n");
                        }
                        
                        if (j < 5) continue;

                        if (strcmp(c[0], target_factory) == 0 || 
                            (strcmp(c[0], "-") == 0 && strcmp(c[1], target_factory) == 0)) {
                            
                            NodeLookup *existing_child = (NodeLookup*)avl_search(lookup_tree, c[2], search_node_lookup);
                            if (existing_child) continue;
                            
                            NodeLookup *parent = (NodeLookup*)avl_search(lookup_tree, c[1], search_node_lookup);
                            if (parent) {
                                NetworkNode *child = calloc(1, sizeof(NetworkNode));
                                if (!child) continue;
                                child->id = strdup(c[2]);
                                if (!child->id) { free(child); continue; }
                                
                                ChildNode *new_c = malloc(sizeof(ChildNode));
                                if (!new_c) { free(child->id); free(child); continue; }
                                new_c->node = child;
                                new_c->leak_percentage = atof(c[4]);
                                new_c->next = parent->node_ptr->children;
                                
                                parent->node_ptr->children = new_c;
                                parent->node_ptr->num_children++;
                                
                                NodeLookup *new_l = malloc(sizeof(NodeLookup));
                                if (!new_l) continue;
                                new_l->id = strdup(c[2]);
                                if (!new_l->id) { free(new_l); continue; }
                                new_l->node_ptr = child;
                                lookup_tree = avl_insert(lookup_tree, new_l, compare_node_lookup);
                                
                                nodes_added = true;
                            }
                        }
                    }
                    fclose(data_file);
                }

                fprintf(stderr, "DEBUG: Network built in %d iterations\n", iteration);

                double loss = compute_network_leaks(net_root, f_data->real_volume);
                out = fopen("leaks.dat", "a");
                if (out) {
                    fprintf(out, "%s;%.3f\n", target_factory, loss / 1000.0);
                    fclose(out);
                }
                
                avl_free(lookup_tree, free_lookup);
                free_network(net_root);
            }
            i++;
        }
        
        else {
            fprintf(stderr, "Error: Unknown argument '%s'\n", argv[i]);
            avl_free(root, free_factory);
            return 1;
        }
    }

    avl_free(root, free_factory);
    return 0;
}
