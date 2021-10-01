#include <stdio.h>

#include "stack.h"

#define print_info printf ("%d %d\n", err, is_valid_stack (&stack_int))

int main () {
    Stack stack_int = {};
    RetErr err = OK;
    
    stack_ctor (&stack_int, 10, sizeof(int), &err);
    print_info;
    
    stack_pop (&stack_int, &err);
    print_info;

    stack_ctor (&stack_int, 1ll << 55, 4, &err);
    print_info;
    
    stack_dtor (&stack_int, &err);
    print_info;
    
    stack_ctor (&stack_int, 1ll << 55, 4, &err);
    print_info;

    stack_dtor (&stack_int, &err);
    print_info;

    err = OK;

    stack_ctor (&stack_int, 10, sizeof(int), &err);
    print_info;

    stack_int.begin_canary = 123;
    int x = 3;

    stack_push (&stack_int, &x, &err);
    print_info;


    return 0;
}
