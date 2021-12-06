#include "akinator.h"

#include <string.h>

void play_akin (AkinTree *tree)
    {
    assert (tree);

    if (tree->type == AT_QUESTION)
        {
        printf ("Does your object have this property?\n %s\nYes(y)    No(n)\n", tree->data);
        char ans = ask_yes_no ("Does it have this property?\n");            
        if (ans == 'y')
            play_akin (tree->left);
        else
            play_akin (tree->right);
        
        return;
        }
    
    // AT_ANSWER
    printf ("Is it %s?\n", tree->data);
    char ans = ask_yes_no ("");
    if (ans == 'y')
        {
        printf ("It is %s i always knew it!!!\n", tree->data);
        return;
        }
    
    // wrong thing
    char *name = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name)), 
         *question = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name));
    
    printf ("It is sorry to hear, but i will become stronger! Put its name here\n");
    fgets (name, MAX_AKIN_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0; // removes \n at end

    printf ("I read |%s|\n", name);

    printf ("Why is it different from %s? Choose what your thing has and mine doesn't have\n", tree->data);
    fgets (question, MAX_AKIN_NAME_LEN, stdin);
    question[strcspn(question, "\n")] = 0; // removes \n at end

    printf ("I read |%s|\n", question);

    tree->left = new_akin_tree (AT_ANSWER, tree, name);
    tree->right = new_akin_tree (AT_ANSWER, tree, tree->data);
    
    tree->data = question;
    tree->type = AT_QUESTION;
    }

void clear_input_buffer () 
    {
    // clears input buffer until the end of line
    while (getchar () != '\n') { ; }
    }

char ask_yes_no (const char *message)
    {
    int is_ok = 1;
    do  {
        fputs (message, stdout);
        char ans = getchar ();
        clear_input_buffer ();
        
        if (tolower (ans) == 'y')
            return 'y';
        else if (tolower (ans == 'n'))
            return 'n';
        else 
            {
            printf ("You wrote some nonsense! Try again, only starting with 'n' or 'y'\n");
            is_ok = 0;
            }
        } while (!is_ok);
    return '\0';
    }

