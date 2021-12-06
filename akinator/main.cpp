#include "akin_tree.h"

#include "../onegin/file_utils.h"

#include <stdio.h>

int main ()
    {
    FILE *base_file = fopen_err ("akin_db.base", "r");
    if (!base_file)
        return 0;
    FileText base = read_text_file (base_file);
    fclose (base_file);
    StringRef *temp = base.strings; 
    AkinTree *tree = read_akin_node (&temp);

    akin_tree_dump (tree);

    free_akin_tree (tree);
    free_file_text (&base);
    }
