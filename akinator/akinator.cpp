#include "speak.h"
#include "akinator.h"

#include "../stack/source/stack.h"

#include <string.h>

#ifndef NDEBUG
#define DEB(...) printf (__VA_ARGS__)
#else
#define DEB(...)
#endif

void akinator_app ()
    {
    // TODO divide into smaller functions
    FILE *base_file = fopen_err ("akin_db.base", "r");
    if (!base_file)
        return;
    FileText base = read_text_file (base_file);
    fclose (base_file);
    StringRef *temp = base.strings; 
    AkinTree *tree = read_akin_node (&temp);

    printf ("\n\n");
    akin_tree_dump (tree);
    printf ("\n\n");

    AT_SAY ("Самый умный акинтор приветствует тебя! У меня маленькая база данных, но я всё равно умнее тебя!\n\n");
    char ans = ask_yes_no ("Хочешь поиграть?\n");
    
    if (ans == 'y')
        {
        while (true)
            {
            play_akin (tree);
            int ans = ask_yes_no ("Хочешь поиграть еще раз?\n");
            if (ans != 'y')
                break;
            }

        AT_SAY ("\nХорошо, теперь дерево такое:\n\n");
        akin_tree_dump (tree);

        AT_SAY ("\n\nХочешь сохранить базу данных в файл?\n");
        ans = ask_yes_no ("Сохранить? ");
        if (ans == 'y')
            {
            FILE *f_out = fopen ("akin_db.base", "w");
            write_tree_to_base (tree, f_out);
            AT_SAY ("Закончила сохранение\n");
            }
        }
    

    AT_SAY ("Хочешь найти определение?\n");
    ans = ask_yes_no ("");
    if (ans == 'y')
        {
        char *name = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name));
        while (true)
            {            
            AT_SAY ("Введи имя: ");
            fgets (name, MAX_AKIN_NAME_LEN, stdin);
            name[strcspn(name, "\n")] = 0; // removes \n at end

            akin_seek_def (tree, name);
            
            int ans = ask_yes_no ("Хочешь найти определение еще раз?\n");
            if (ans != 'y')
                break;
            }
        free (name);
        }

    AT_SAY ("Хочешь узнать в чем отличие?\n");
    ans = ask_yes_no ("");
    if (ans == 'y')
        {
        char *name1 = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name1));
        char *name2 = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name2));
    
        while (true)
            {
            AT_SAY ("Введи первое имя: ");
            fgets (name1, MAX_AKIN_NAME_LEN, stdin);
            name1[strcspn(name1, "\n")] = 0; // removes \n at end
            
            AT_SAY ("Введи второе имя: ");
            fgets (name2, MAX_AKIN_NAME_LEN, stdin);
            name2[strcspn(name2, "\n")] = 0; // removes \n at end

            akin_diff_def (tree, name1, name2);

            int ans = ask_yes_no ("Хочешь найти разницу еще раз?\n");
            if (ans != 'y')
                break;
            }

        free (name1);
        free (name2);
        }

    AT_SAY ("Хочешь визуальный дамп?\n");
    ans = ask_yes_no ("");
    if (ans == 'y')
        {
        akin_tree_graph_dump (tree);
        }

    AT_SAY ("Пока! Удачи на сессии!\n");
    free_akin_tree (tree);
    free_file_text (&base);
    }

void play_akin (AkinTree *tree)
    {
    assert (tree);

    if (tree->type == AT_QUESTION)
        {
        AT_SAY ("%s - это о том что ты загадал?\n", tree->data);
        char ans = ask_yes_no ("");            
        if (ans == 'y')
            play_akin (tree->left);
        else
            play_akin (tree->right);
        
        return;
        }
    
    // AT_ANSWER
    AT_SAY ("Это %s?\n", tree->data);
    char ans = ask_yes_no ("");
    if (ans == 'y')
        {
        AT_SAY ("Ура, я всегда знала что это %s!!!\n", tree->data);
        return;
        }
    
    // wrong thing
    char *name = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name)), 
         *question = (char*) calloc (MAX_AKIN_NAME_LEN, sizeof (*name));
    
    // TODO move adding element to another function
    AT_SAY ("Это очень грустно, но я стану сильнее! Введи имя того что ты загадал: ");
    fgets (name, MAX_AKIN_NAME_LEN, stdin);
    name[strcspn(name, "\n")] = 0; // removes \n at end

    AT_SAY ("Чем оно отличается от %s? Введи свойство, которого нет у того что я отгадала\n", tree->data);
    fgets (question, MAX_AKIN_NAME_LEN, stdin);
    question[strcspn(question, "\n")] = 0; // removes \n at end

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
        if (message[0] != 0)
            AT_SAY ("%s", message);
        char ans = getchar ();
        clear_input_buffer ();
        
        if (tolower (ans) == 'y' || ans == 'д' || ans == 'Д')
            return 'y';
        else if (tolower (ans == 'n') || ans == 'н' || ans == 'Н')
            return 'n';
        else 
            {
            AT_SAY ("Ты написал какой-то бред, ответ может начинаться только с д, н, n или y\n");
            is_ok = 0;
            }
    } while (!is_ok);

    return '\0';
    }

void akin_seek_def (AkinTree *tree, const char *name, Stack *stk_names, Stack *stk_is)
    {
    assert (tree);
    assert (name);

    bool to_free_names = 0, to_free_is = 0;
    if (stk_names == nullptr)
        {
        stk_names = (Stack *) calloc (1, sizeof (*stk_names));
        stack_ctor (stk_names, 1, sizeof (char*));
        to_free_names = 1;
        }
    
    if (stk_is == nullptr)
        {
        stk_is = (Stack *) calloc (1, sizeof (*stk_is));
        stack_ctor (stk_is, 1, sizeof (int));
        to_free_is = 0;
        }

    akin_seek_def_stack (tree, name, stk_names, stk_is);

    if (to_free_names)
        {
        stack_dtor (stk_names);
        free (stk_names);
        }

    if (to_free_is)
        {
        stack_dtor (stk_is);
        free (stk_is);
        }
    }

int akin_seek_def_stack (AkinTree *tree, const char *name, Stack *stk_names = nullptr, Stack *stk_is = nullptr)
    {
    assert (tree);
    assert (name);
    assert (stk_names);
    assert (stk_is);

    if (tree->type == AT_QUESTION)
        {
        int is_yes = 1;

        stack_push (stk_names, &tree->data);
        stack_push (stk_is, &is_yes);
        if (akin_seek_def_stack (tree->left, name, stk_names, stk_is))
            return 1;
        stack_pop (stk_is);

        is_yes = 0;
        stack_push (stk_is, &is_yes);
        if (akin_seek_def_stack (tree->right, name, stk_names, stk_is))
            return 1;
        
        stack_pop (stk_is);
        stack_pop (stk_names);
        
        return 0;
        }

    // AT_ANSWER
    if (strcmp (name, tree->data) != 0)
        // wrong name
        return 0;
    
    // TODO move print definiton to function (stk_names, st_is)
    DEB ("I found it here\n");
    const char **names = (const char**) stk_names->arr;
    const int *status = (const int*) stk_is->arr;
    
    AT_SAY ("%s это кто-то ", name);
    for (int i = 0; i < stk_is->size; i++)
        if (status[i])
            AT_SAY ("которое %s ", names[i]);
        else
            AT_SAY ("которое не %s ", names[i]);

    AT_SAY ("и всё.\n");
    return 1;
    }

void akin_diff_def (AkinTree *tree, const char *name1, const char *name2)
    {
    assert (tree);
    assert (name1);
    assert (name2);

    // TODO why so much copy-paste?
    Stack *stk_names1 = (Stack *) calloc (1, sizeof (*stk_names1));
    stack_ctor (stk_names1, 1, sizeof (char*));
    Stack *stk_is1 = (Stack *) calloc (1, sizeof (*stk_is1));
    stack_ctor (stk_is1, 1, sizeof (int));

    Stack *stk_names2 = (Stack *) calloc (1, sizeof (*stk_names2));
    stack_ctor (stk_names2, 1, sizeof (char*));
    Stack *stk_is2 = (Stack *) calloc (1, sizeof (*stk_is2));
    stack_ctor (stk_is2, 1, sizeof (int));

    // TODO how to avoid this copy-paste with free?
    if (!akin_seek_def_stack (tree, name1, stk_names1, stk_is1))
        {
        AT_SAY ("Я не нашла, кто такой %s, поэтому не буду искать разницу\n", name1);

        stack_dtor (stk_names1);
        free (stk_names1);
        stack_dtor (stk_is1);
        free (stk_is1);
        
        stack_dtor (stk_names2);
        free (stk_names2);
        stack_dtor (stk_is2);
        free (stk_is2);
        
        return;
        }
    if (!akin_seek_def_stack (tree, name2, stk_names2, stk_is2))
        {
        AT_SAY ("Я не нашла, кто такой %s, поэтому не буду искать разницу\n", name2);

        stack_dtor (stk_names1);
        free (stk_names1);
        stack_dtor (stk_is1);
        free (stk_is1);
        
        stack_dtor (stk_names2);
        free (stk_names2);
        stack_dtor (stk_is2);
        free (stk_is2);
        
        return;
        }

    DEB ("I found definitions, now i go next\n");

    stack_dump (stk_names1, stdout, MAX_DUMP);
    stack_dump (stk_is1, stdout, MAX_DUMP);
    stack_dump (stk_names2, stdout, MAX_DUMP);
    stack_dump (stk_is2, stdout, MAX_DUMP);
    
    const char **names1 = (const char**) stk_names1->arr;
    const int *status1 = (const int*) stk_is1->arr;
    
    const char **names2 = (const char**) stk_names2->arr;
    const int *status2 = (const int*) stk_is2->arr;

    // printf ("%s and %s are same at such things as: ", name1, name2);

    int i = 0;
    if (*status1 == *status2)
        {
        AT_SAY ("%s и %s схожи тем, что они кто-то ", name1, name2);
        while (status1[i] == status2[i] && i < stk_is1->size && i < stk_is2->size)
            {
                if (status1[i])
                    AT_SAY ("которое %s ", names1[i]);
                else
                    AT_SAY ("которое не %s ", names1[i]);
                i++;
            }
        AT_SAY ("НО! они отличаются тем, что \n");
        }
    else
        AT_SAY ("%s и %s не имеют ничего общего и отличаются тем, что \n", name1, name2);
    
    // TODO see todo "move print definiton..."
    int begin_diff = i;
    AT_SAY ("%s это кто-то ", name1);
    for (; i < stk_is1->size; i++)
        if (status1[i])
            AT_SAY ("которое %s ", names1[i]);
        else
            AT_SAY ("которое не %s ", names1[i]);

    i = begin_diff;
    AT_SAY ("\nНО %s это кто-то ", name2);
    for (; i < stk_is2->size; i++)
        if (status2[i])
            AT_SAY ("которое %s ", names2[i]);
        else
            AT_SAY ("которое не %s ", names2[i]);    

    AT_SAY ("\nИ эта вся разница между ними\n");

    stack_dtor (stk_names1);
    free (stk_names1);
    stack_dtor (stk_is1);
    free (stk_is1);
    
    stack_dtor (stk_names2);
    free (stk_names2);
    stack_dtor (stk_is2);
    free (stk_is2);
    }



