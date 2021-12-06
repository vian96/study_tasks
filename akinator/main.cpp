#include "akin_tree.h"
#include "akinator.h"

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

    printf ("\n\n");
    akin_tree_dump (tree);
    printf ("\n\n");

    printf ("Hello to the smartest akinator! I don't have db but i am smart!\n\n");
    
    while (true)
        {
        play_akin (tree);
        int ans = ask_yes_no ("Wanna play again? Type 'y' if so\n");
        if (ans != 'y')
            break;
        }

    printf ("\nOkay, now tree is:\n\n");
    akin_tree_dump (tree);

    printf ("\n\nDo you want to save it in database file?\n");
    char ans = ask_yes_no ("Save? ");
    if (ans == 'y')
        {
        FILE *f_out = fopen ("akin_db_save.base", "w");
        write_tree_to_base (tree, f_out);
        printf ("Done writing\n");
        }

    printf ("Goodbye!\n");
    free_akin_tree (tree);
    free_file_text (&base);
    }
