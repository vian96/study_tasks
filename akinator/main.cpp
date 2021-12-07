#include "akin_tree.h"
#include "akinator.h"

#include "../onegin/file_utils.h"
#include "../stack/source/stack.h"

#include <stdio.h>
#include <string.h>

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
    char ans = ask_yes_no ("Want to play?\n");
    
    if (ans == 'y')
        {
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
        ans = ask_yes_no ("Save? ");
        if (ans == 'y')
            {
            FILE *f_out = fopen ("akin_db_save.base", "w");
            write_tree_to_base (tree, f_out);
            printf ("Done writing\n");
            }
        }
    

    printf ("Want to find definition?\n");
    ans = ask_yes_no ("");
    if (ans == 'y')
        {
        char *name = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name));
        
        printf ("Put its name here\n");
        fgets (name, MAX_AKIN_NAME_LEN, stdin);
        name[strcspn(name, "\n")] = 0; // removes \n at end

        Stack *stk_yes = (Stack*) calloc (1, sizeof (*stk_yes));
        stack_ctor (stk_yes, 10, sizeof (char*));
        
        Stack *stk_no = (Stack*) calloc (1, sizeof (*stk_no));
        stack_ctor (stk_no, 10, sizeof (char*));

        akin_seek_definition (tree, name, stk_yes, stk_no);
        
        free (name);
        stack_dtor (stk_yes);
        stack_dtor (stk_no);
        free (stk_yes);
        free (stk_no);
        }

    printf ("Goodbye!\n");
    free_akin_tree (tree);
    free_file_text (&base);
    }
