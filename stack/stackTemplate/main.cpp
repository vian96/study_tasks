#include <stdio.h>

#include "stackT.h"

StackTemplate(int)

StackTemplate(char)

int main () {
    Stack_int stk = {}; // int 
    RetErr err = OK;

    printf("/////////////// INT ///////////////\n");
    stack_ctor_int (&stk, 10);
    
    for (int i = 7; i < 15; i++){
        stack_push_int (&stk, i, &err);
        //printf ("%d %d\n", i, err);
    }

    printf ("%d %d %d\n", stk.arr, stk.capacity, stk.size);

    err = OK; 
    size_t old = stk.capacity;

    stack_dump_int (&stk, stdout, print_stack_int);
    
    stack_dtor_int (&stk);

    printf ("%d %d %d\n", stk.arr, stk.capacity, stk.size);

    printf("/////////////// char ///////////////\n");

    Stack_char stk_ch = {};

    stack_ctor_char (&stk_ch, 10);
    
    for (int i = 0; i < 10; i++){
        stack_push_char (&stk_ch, i + 'A', &err);
        //printf ("%d %d\n", i, err);
    }

    printf ("%d %d %d\n", stk_ch.arr, stk_ch.capacity, stk_ch.size);

    err = OK; 
    old = stk_ch.capacity;

    stack_dump_char (&stk_ch, stdout, print_stack_char);
    
    stack_dtor_char (&stk_ch);

    printf ("%d %d %d\n", stk_ch.arr, stk_ch.capacity, stk_ch.size);

    printf ("DONE");
}