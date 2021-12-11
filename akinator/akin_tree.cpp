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


