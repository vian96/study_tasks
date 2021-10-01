#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#define ADD_ERR(err, value) ((err) && (*(err) = (RetErr)((int)(*(err)) | (int)(value))))

#define CANARY_PROTECTION
#define CANARY_ALIGNMENT

const size_t MIN_STACK_SIZE = 64;

// TODO do function change_capacity
struct Stack {
    uint64_t begin_canary;

    size_t size;
    size_t capacity;
    size_t size_el;
    void *arr;
    
    uint64_t hash;

    uint64_t end_canary;
};

enum RetErr {
    // TODO change to STACK_OK
    OK = 0, 
    OVERFLOW = 1, 
    UNDERFLOW = 1 << 1,
    NO_MEMORY = 1 << 2, 
    INVALID_STACK = 1 << 3, 
    NULL_PTR = 1 << 4,
    INVALID_ARG = 1 << 5, 
};

void *realloc_ (void *ptr, const size_t len, const size_t size);

void print_stack_int (const void *elem, FILE *f_out, RetErr *err = nullptr);

// constructor and desctructor
void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err = nullptr);

void stack_dtor (Stack *stack, RetErr *err = nullptr);

// element access functions
void stack_push (Stack *stack, const void *value, RetErr *err = nullptr);

void *stack_pop (Stack *stack, RetErr *err = nullptr);

void *stack_top (const Stack *stack, RetErr *err = nullptr);

// change of stack
void stack_shrink_to_fit (Stack *stack, RetErr *err = nullptr);

// debug functions
void stack_dump (Stack *stack, FILE *f_out, void (* print) (const void *elem, FILE *f_out, RetErr *err), RetErr *err = nullptr);

// stack tests
bool check_stack (const Stack *stack, RetErr *err);

bool check_null (const void *ptr, RetErr *err);

bool is_valid_stack (const Stack *stack);

bool is_empty_stack (const Stack *stack, RetErr *err = nullptr);

