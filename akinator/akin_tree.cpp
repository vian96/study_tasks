#include "akin_tree.h"

#include "../asm/asm/string_utils.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef NDEBUG
#define DEB(...) printf(__VA_ARGS__)
#else
#define DEB(...)
#endif

void akin_tree_ctor (AkinTree *tree, AkinTreeType type, AkinTree *parent, char *data, 
                                                        AkinTree *left, AkinTree *right)
    {
    assert (tree);

    tree->parent = parent;
    tree->type = type;
    tree->data = data;
    tree->left = left;
    tree->right = right;
    }
    
AkinTree *new_akin_tree (AkinTreeType type, AkinTree *parent, char *data, AkinTree *left, AkinTree *right)
    {
    AkinTree *tree = (AkinTree*) calloc (1, sizeof (*tree));
    akin_tree_ctor (tree, type, parent, data, left, right);
    return tree;
    }

AkinTree *read_akin_node (StringRef **line, AkinTree *parent) 
    {
    assert (line);

    const char *str = (*line)->begin;
    skip_blank (&str);

    AkinTreeType type = AT_ANSWER;
    if (*str == '?')
        type = AT_QUESTION;
    
    DEB ("Met line %s with type %d\n", str, type);

    AkinTree *tree = new_akin_tree (type, parent, (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (char)));
    if (type == AT_QUESTION)
        strcpy (tree->data, str + 1);
    else
        strcpy (tree->data, str);

    (*line)++;

    if (type == AT_QUESTION)
        {
        tree->left = read_akin_node (line, tree);
        tree->right = read_akin_node (line, tree);
        }
    
    return tree;
    }

void free_akin_tree (AkinTree *tree)
    {
    assert (tree);
    
    if (tree->left != nullptr)
        free_akin_tree (tree->left);

    if (tree->right != nullptr)
        free_akin_tree (tree->right);
    
    free (tree->data);
    free (tree);
    }

void akin_tree_dump (AkinTree *tree, int depth)
    {
    assert (tree);

    printf ("%*sAkinator tree with data %s\n", depth, "", tree->data);
    //DEB ("%*stype: %d, left: %p, right: %p\n", depth, "", tree->type, tree->left, tree->right);
    if (tree->type == AT_QUESTION)
        {
        printf ("%*swith something in here:\n", depth, "");
        akin_tree_dump (tree->left, depth + 4);
        akin_tree_dump (tree->right, depth + 4);
        }
    }

void akin_tree_graph_dump (AkinTree *tree)
    {
    assert (tree);
    
    DEB ("Creating file for graphviz\n");
    FILE *gv_out = fopen("akin_dump.gv", "w");
    DEB ("Opened graphviz file\n");

    fprintf(gv_out, "# This is automatically generated dump of akinator\n"
                    "digraph Tree{\n\n"
                    "\trankdir=UD;\n\n"
                    "\tnode[color=\"red\",fontsize=14];\n\n");

    akin_tree_graph_node (tree, gv_out);
    fprintf(gv_out, "\n");

    akin_tree_graph_arrow (tree, gv_out);
    fprintf(gv_out, "\n");
    fprintf(gv_out, "}");

    fclose(gv_out);
    
    // TODO it doesn't work because of enccoding of file
    // system("dot -Tpng akin_dump.gv -o akin_dump.png");
    }

void akin_tree_graph_node (AkinTree *tree, FILE *f_out)
    {
    assert (tree);
    assert (f_out);

    fprintf(f_out, "\t%lu[shape=record, style=\"filled\", fillcolor=\"yellow\", label=\"%s\"];\n", 
            tree, tree->data);

    if (tree->left != nullptr) 
        akin_tree_graph_node (tree->left, f_out);
    if (tree->right != nullptr) 
        akin_tree_graph_node (tree->right, f_out);
    }

void akin_tree_graph_arrow (AkinTree *tree, FILE *f_out)
    {
    assert (tree);
    assert (f_out);

    if (tree->left != nullptr && tree->right != nullptr)
        {
        fprintf(f_out, "\t%lu -> %lu[color=\"green\", label=\"NO\", fontsize=12]\n", tree, tree->left);
        fprintf(f_out, "\t%lu -> %lu[color=\"blue\", label=\"YES\", fontsize=12]\n", tree, tree->right);
        }

    if (tree->left) 
        akin_tree_graph_arrow (tree->left, f_out);
    if (tree->right)
        akin_tree_graph_arrow (tree->right, f_out);
    }

void write_tree_to_base (AkinTree *tree, FILE *f_out, int depth)
    {
    assert (f_out);
    
    fprintf (f_out, "%*s", depth, "");
    if (tree->type == AT_QUESTION)
        fputc ('?', f_out);
    fprintf (f_out, "%s\n", tree->data);
    
    if (tree->type == AT_QUESTION)
        {
        write_tree_to_base (tree->left, f_out, depth + 4);
        write_tree_to_base (tree->right, f_out, depth + 4);
        }
    }


