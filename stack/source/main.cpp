#include <stdio.h>

#include "stack.h"


int main () {
    Stack stk = {}; // int 
    stack_ctor (&stk, 10, sizeof (int));
    
    RetErr err = OK;
    for (int i = 7; i < 15; i++){
        stack_push (&stk, &i, &err);
        //printf ("%d %d\n", i, err);
    }

    printf ("%d %d %d %d\n", stk.arr, stk.capacity, stk.size, stk.size_el);
    printf ("\n\n\n");

    err = OK; 
    size_t old = stk.capacity;

    stack_dump (&stk, stdout, print_stack_int);

    while (err == OK) {
        int *x = (int*) stack_pop (&stk, &err);
        
        if (!err && old != stk.capacity) {
            printf ("%d : %d -> %d\n", *x, old, stk.capacity);
            old = stk.capacity;
        }

        //if (!err) 
            //printf ("%d\n", *x);
    }

    printf ("%d %d %d %d\n", stk.arr, stk.capacity, stk.size, stk.size_el);
    
    stack_dtor (&stk);

    printf ("%d %d %d %d\n", stk.arr, stk.capacity, stk.size, stk.size_el);

    printf ("DONE");
}