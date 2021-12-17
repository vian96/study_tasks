#ifndef AKINATOR_AKIN_TREE_H
#define AKINATOR_AKIN_TREE_H

#include "../onegin/file_utils.h"

const size_t MAX_AKIN_NAME_LEN = 100;

enum AkinTreeType
    {
    AT_ANSWER = 1,
    AT_QUESTION = 2,
    };

struct AkinTree 
    {
    AkinTreeType type;
    AkinTree *parent;
    char *data;
    // left is positive answer
    AkinTree *left, *right;
    };

void akin_tree_ctor (AkinTree *tree, AkinTreeType type, AkinTree *parent = nullptr, 
                     char *data = nullptr, AkinTree *left = nullptr, AkinTree *right = nullptr);

AkinTree *new_akin_tree (AkinTreeType type, AkinTree *parent = nullptr, 
                            char *data = nullptr, AkinTree *left = nullptr, AkinTree *right = nullptr);

AkinTree *read_akin_node (StringRef **line, AkinTree *parent = nullptr);

// TODO change to dtor
void free_akin_tree (AkinTree *tree);

void akin_tree_dump (AkinTree *tree, int depth = 0);

void akin_tree_graph_dump (AkinTree *tree);

void akin_tree_graph_node (AkinTree *tree, FILE *f_out);

void akin_tree_graph_arrow (AkinTree *tree, FILE *f_out);

void write_tree_to_base (AkinTree *tree, FILE *f_out, int depth = 0);

#endif // AKINATOR_AKIN_TREE_H
