#include "akinator.h"

#include "../stack/source/stack.h"

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

void akin_seek_definition (AkinTree *tree, const char *name, Stack *stk_yes, Stack *stk_no)
    {
    assert (tree);
    assert (name);
    assert (stk_yes);
    assert (stk_no);

    if (tree->type == AT_QUESTION)
        {
        stack_push (stk_yes, &tree->data);
        akin_seek_definition (tree->left, name, stk_yes, stk_no);
        stack_pop (stk_yes);

        stack_push (stk_no, &tree->data);
        akin_seek_definition (tree->right, name, stk_yes, stk_no);
        stack_pop (stk_no);
        
        // printf ("Popped %s\n", * (char**) stack_pop (stk));    
        return;
        }

    // AT_ANSWER
    if (strcmp (name, tree->data) != 0)
        {
        // wrong name
        return;
        }
    
    printf ("I found it here\n");
    const char **tmp = (const char**) stk_yes->arr;
    printf ("%s is something ", name);
    for (int i = 0; i < stk_yes->size; i++)
        printf ("that %s ", *(tmp + i));
        
    tmp = (const char**) stk_no->arr;
    printf ("BUT ", name);
    for (int i = 0; i < stk_no->size; i++)
        printf ("that not %s ", *(tmp + i));

    printf ("that's it.\n");
    }


