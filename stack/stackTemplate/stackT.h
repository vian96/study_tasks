// WARNING: this is joke code

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const size_t MIN_STACK_SIZE = 64;

enum RetErr {
    // TODO change to STACK_OK
    OK = 0, 
    OVERFLOW = 1, UNDERFLOW = 2,
    NO_MEMORY = 3, 
    INVALID_ARG = 4, 
};

void *realloc_ (void *ptr, size_t len, size_t size) {
    return realloc (ptr, len * size);
}

void print_stack_int (const int elem, FILE *f_out, RetErr *err = nullptr) {
    if (!elem || !f_out) {
        if (err)
            *err = INVALID_ARG;
        return;
    }

    fprintf (f_out, "%d ", elem);\
}

void print_stack_char (const char elem, FILE *f_out, RetErr *err = nullptr) {
    if (!elem || !f_out) {
        if (err)
            *err = INVALID_ARG;
        return;
    }

    fprintf (f_out, "%c ", elem);\
}

// to avoid misusing another function
#define malloc() (void)(0)
#define realloc() (void)(0)

#define StackTemplate(ElemT) \
struct Stack_##ElemT {  \
    size_t size;    \
    size_t capacity;    \
    ElemT *arr;  \
};\
\
bool is_valid_stack_##ElemT (Stack_##ElemT *stack) { \
    return stack && stack->size <= stack->capacity; \
} \
\
bool is_empty_stack_##ElemT (Stack_##ElemT *stack, RetErr *err = nullptr) {\
    /* if stack is empty it will not be valid because size_el == 0*/\
    if (!is_valid_stack_##ElemT(stack)) {\
        if (err)\
            *err = INVALID_ARG;\
        return false;\
    }\
\
    return !stack->arr && !stack->capacity && !stack->size;\
}\
\
void stack_dump_##ElemT (Stack_##ElemT *stack, FILE *f_out, void (* print) (const ElemT elem, FILE *f_out, RetErr *err), RetErr *err = nullptr) {\
    if (!is_valid_stack_##ElemT(stack) || !f_out || ferror(f_out) || !print) {\
        printf("ERR %d %d %d %d\n", stack, stack->size, stack->capacity, !print);\
        if (err)\
            *err = INVALID_ARG;\
        return;\
    }\
\
    if (err)\
        *err = OK;\
        \
    for (int i = 0; i < stack->size; i++) {\
        print (stack->arr[i], f_out, err);\
        if (err && *err)\
            return;\
    }\
    fputc ('\n', f_out);\
}\
\
void stack_ctor_##ElemT (Stack_##ElemT *stack, size_t capacity, RetErr *err = nullptr) {\
    if (!is_empty_stack_##ElemT (stack)) {\
        if (err)\
            *err = INVALID_ARG;\
        return;\
    }\
    \
    stack->capacity = capacity;\
    \
    /* TODO small string optimization*/\
\
    if (capacity) {\
        if (capacity < MIN_STACK_SIZE)\
            capacity = MIN_STACK_SIZE;\
        \
        stack->arr = (ElemT *) calloc (capacity, sizeof (ElemT));\
        stack->capacity = capacity;\
\
        if (!stack->arr && err)\
            *err = NO_MEMORY;\
    }\
}\
\
void stack_dtor_##ElemT (Stack_##ElemT *stack, RetErr *err = nullptr) {\
    if (!stack) {\
        if (err)\
            *err = INVALID_ARG;\
        return;\
    }\
\
    free (stack->arr);\
\
    stack->arr = nullptr;\
    stack->capacity = 0;\
    stack->size = 0;\
}\
\
void stack_push_##ElemT (Stack_##ElemT *stack, const ElemT value, RetErr *err = nullptr) {\
    if (!is_valid_stack_##ElemT(stack) || !value) {\
        if (err)\
            *err = INVALID_ARG;\
        return;\
    }\
\
    if (!stack->capacity)\
        stack->capacity = MIN_STACK_SIZE;\
\
    if (stack->size == stack->capacity) {\
        stack->capacity *= 2;\
        void *temp = realloc_ (stack->arr, stack->capacity, sizeof (ElemT));\
        \
        if (!temp) { \
            if (err)\
                *err = NO_MEMORY;\
            \
            return;\
        }\
\
        stack->arr = (ElemT *) temp;\
    }\
\
    stack->arr[stack->size++] = value;\
}\
\
ElemT stack_pop_##ElemT (Stack_##ElemT *stack, RetErr *err = nullptr) {\
    if (!is_valid_stack_##ElemT(stack)) {\
        if (err)\
            *err = INVALID_ARG;\
        return NULL;\
    }\
\
    if (!stack->size) {\
        /* TODO should i change if to (err && *err = ...)*/\
        if (err)\
            *err = UNDERFLOW;\
\
        return ElemT();\
    }\
\
    if (stack->size <= stack->capacity / 4 && stack->capacity >= 2 * MIN_STACK_SIZE) {\
        stack->capacity = stack->capacity / 2;\
\
        stack->arr = (ElemT *) realloc_ (stack->arr, stack->capacity, sizeof(ElemT));\
    }\
\
    return stack->arr[--(stack->size)]; \
}\
\
ElemT stack_top_##ElemT (Stack_##ElemT *stack, RetErr *err = nullptr) {\
    if (!is_valid_stack_##ElemT(stack)) {\
        if (err)\
            *err = INVALID_ARG;\
        return ElemT();\
    }\
\
    if (!stack->size) {\
        /* TODO should i change if to (err && *err = ...)*/\
        if (err)\
            *err = UNDERFLOW;\
\
        return ElemT();\
    }\
\
    return stack->arr[stack->size - 1]; \
}\

#undef malloc
#undef realloc
