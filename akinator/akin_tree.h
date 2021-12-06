#include "../onegin/file_utils.h"

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
    AkinTree *left, *right;
    };

void akin_tree_ctor (AkinTree *tree, AkinTreeType type, AkinTree *parent = nullptr, 
                     char *data = nullptr, AkinTree *left = nullptr, AkinTree *right = nullptr);

AkinTree *new_akin_tree (AkinTreeType type, AkinTree *parent = nullptr, 
                            char *data = nullptr, AkinTree *left = nullptr, AkinTree *right = nullptr);

AkinTree *read_akin_node (StringRef **line, AkinTree *parent = nullptr);

// TODO change to dtor
// WARNING: it does not frees strings because it doesn't copy it
void free_akin_tree (AkinTree *tree);

void akin_tree_dump (AkinTree *tree, int depth = 0);
