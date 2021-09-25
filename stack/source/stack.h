#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const size_t MIN_STACK_SIZE = 64;

// TODO ? hide members in .cpp
// TODO do function change_capacity
struct Stack {
    size_t size;
    size_t capacity;
    size_t size_el;
    void *arr;
};

enum RetErr {
    // TODO change to STACK_OK
    OK = 0, 
    OVERFLOW = 1, UNDERFLOW = 2,
    NO_MEMORY = 3, 
    INVALID_ARG = 4, 
};

// utils functions
void *realloc_ (void *ptr, size_t len, size_t size);

bool is_valid_stack (Stack *stack);

bool is_empty_stack (Stack *stack, RetErr *err = nullptr);

void stack_dump (Stack *stack, FILE *f_out, void (* print) (const void *elem, FILE *f_out, RetErr *err), RetErr *err = nullptr);

void print_stack_int (const void *elem, FILE *f_out, RetErr *err = nullptr);

// constructor and desctructor
void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err = nullptr);

void stack_dtor (Stack *stack, RetErr *err = nullptr);

// elements access functions
void stack_push (Stack *stack, const void *value, RetErr *err = nullptr);

void *stack_pop (Stack *stack, RetErr *err = nullptr);

void *stack_top (Stack *stack, RetErr *err = nullptr);

// TODO get members of structure access


