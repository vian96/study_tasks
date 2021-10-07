#include <stdio.h>

#include "config.h"

#include "stack.h"

int main () {
    Stack stack_int = {};
    RetErr err = OK;

    stack_ctor (&stack_int, 10, sizeof (int));

    for (int i = 7; i < 15; i++){
        stack_push (&stack_int, &i, &err);
        //printf ("%d %d\n", i, err);
    }

    printf ("%d %d %d %d\n", stack_int.arr, stack_int.capacity, stack_int.size, stack_int.size_el);
    printf ("\n\n\n");

    err = OK; 
    size_t old = stack_int.capacity;

    stack_dump (&stack_int, stdout, MAX_DUMP, print_stack_int);

    while (err == OK) {
        int *x = (int*) stack_pop (&stack_int, &err);

        x && printf ("x: %d ", *x);
        printf("\n");

        if (!err && old != stack_int.capacity) {
            printf ("%d : %d -> %d\n", *x, old, stack_int.capacity);
            old = stack_int.capacity;
        }

        //if (!err) 
            //printf ("%d\n", *x);
    }

    printf ("%d %d %d %d\n", stack_int.arr, stack_int.capacity, stack_int.size, stack_int.size_el);

    stack_dtor (&stack_int);

    printf ("%d %d %d %d\n", stack_int.arr, stack_int.capacity, stack_int.size, stack_int.size_el);

    printf ("DONE");


    return 0;
}
