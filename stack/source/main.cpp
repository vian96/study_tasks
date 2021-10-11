#include <stdio.h>

#include "config.h"

#include "stack.h"

#define deb printf ("DEB %d\n", __LINE__);

int main () {
    Stack stack_int = {};
    RetErr err = OK;

    stack_ctor (&stack_int, 10, sizeof (int));
    stack_dump (&stack_int, stdout, MAX_DUMP, print_stack_int);
    
    for (int i = 7; i < 1240; i++){
        stack_push (&stack_int, &i, &err);

        if (!is_valid_stack (&stack_int)) {
            printf ("ERR: %d %d\n", i, err);
            break;
        }
    }
    
    printf ("%d %d %d %d\n\n", stack_int.arr, stack_int.capacity, stack_int.size, stack_int.size_el);

    err = OK; 
    size_t old = stack_int.capacity;

    stack_dump (&stack_int, stdout, MAX_DUMP, print_stack_int);

    while (err == OK) {
        int *x = (int*) stack_pop (&stack_int, &err);

        //x && printf ("x: %d\n", *x);

        if (!err && old != stack_int.capacity) {
            printf ("%d : %d -> %d\n", *x, old, stack_int.capacity);
            old = stack_int.capacity;
        }
    }

    stack_dump (&stack_int, stdout, MAX_DUMP, print_stack_int);
  
    printf ("%d %d %d %d\n", stack_int.arr, stack_int.capacity, stack_int.size, stack_int.size_el);

    stack_dtor (&stack_int);

    stack_dump (&stack_int, stdout, MAX_DUMP, print_stack_int);
 
    printf ("%d %d %d %d\n", stack_int.arr, stack_int.capacity, stack_int.size, stack_int.size_el);

    printf ("DONE");

    close_log_file ();

    return 0;
}
