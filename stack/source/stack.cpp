#include "stack.h"

// canary protection utils

const uint64_t GOOD_CANARY = ((uint64_t)1000 - 7) * 300/*$*/ * 1488 * 1337 * 666 * 228 * 107;

uint64_t get_good_canary (const Stack *stack) {
    return GOOD_CANARY ^ (uint64_t) stack;
}

// to avoid misusing bad functions

void *realloc_ (void *ptr, const size_t len, const size_t size) {
    return realloc (ptr, len * size);
}

#define malloc() (void)(0)
#define realloc() (void)(0)

void print_stack_int (const void *elem, FILE *f_out, RetErr *err) {
    if (check_null ((void*) (elem && f_out), err))
        return;

    if (!elem || !f_out) {
        ADD_ERR (err, NULL_PTR);
        return;
    }

    fprintf (f_out, "%d ", *(int *) elem);
}

// constructor and destructor

void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err) {
    if (!is_empty_stack (stack, err))
        return;
    
    stack->begin_canary = get_good_canary (stack);
    stack->end_canary = get_good_canary (stack);

    stack->capacity = capacity;
    stack->size_el = size_el;

    if (capacity) {
        if (capacity < MIN_STACK_SIZE)
            capacity = MIN_STACK_SIZE;
        
        stack->arr = calloc (capacity, size_el);
        stack->capacity = capacity;

        if (!stack->arr)
            ADD_ERR (err, NO_MEMORY);
    }
}

void stack_dtor (Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return;

    // TODO what to do if there is allocated memory and there is error ?

    free (stack->arr);

    stack->arr = nullptr;
    stack->capacity = 0;
    stack->size = 0;
    stack->size_el = 0;

    stack->begin_canary = 0;
    stack->end_canary = 0;
}

// element access functions

void stack_push (Stack *stack, const void *value, RetErr *err) {
    if (!check_stack (stack, err))
        return;
    if (check_null (value, err))
        return;

    if (!stack->capacity)
        stack->capacity = MIN_STACK_SIZE;

    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        void *temp = realloc_ (stack->arr, stack->capacity, stack->size_el);
        
        if (!temp) { 
            ADD_ERR (err, NO_MEMORY);
            
            return;
        }

        stack->arr = temp;
    }

    memcpy ((char *)stack->arr + (stack->size++) * (stack->size_el), value, stack->size_el);
}

void *stack_pop (Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return nullptr;

    if (!stack->size) {
        if (err)
            *err = UNDERFLOW;

        return nullptr;
    }

    if (stack->size <= stack->capacity / 4 && stack->capacity >= 2 * MIN_STACK_SIZE) {
        stack->capacity = stack->capacity / 2;

        stack->arr = realloc_ (stack->arr, stack->capacity, stack->size_el);
    }

    return (char *)(stack->arr) + (--(stack->size)) * stack->size_el;
}

void *stack_top (const Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return nullptr;

    if (!stack->size) {
        ADD_ERR (err, UNDERFLOW);

        return nullptr;
    }

    return (char *)(stack->arr) + (stack->size - 1) * stack->size_el;
}

// debug functions

void stack_dump (Stack *stack, FILE *f_out, void (* print) (const void *elem, FILE *f_out, RetErr *err), RetErr *err) {
    if (check_null ((void*) (stack && f_out && print), err))
        return;
    if (!check_stack (stack, err))
        return;
    if (ferror(f_out)) {
        ADD_ERR (err, INVALID_ARG);
        return;
    }

    RetErr t_err = OK;

    for (int i = 0; i < stack->size; i++) {
        print ((char*)stack->arr + i * stack->size_el, f_out, &t_err);
        if (t_err) {
            ADD_ERR (err, t_err);
            return;
        }
    }
    fputc ('\n', f_out);
}

// stack tests
bool check_stack (const Stack *stack, RetErr *err = nullptr) {
    if (!stack) {
        ADD_ERR (err, NULL_PTR);
        return 0;
    }
    if (!is_valid_stack (stack)) {
        ADD_ERR (err, INVALID_STACK);
        return 0;
    }

    return 1;
}

bool check_null (const void *ptr, RetErr *err) {
    if (!ptr) {
        ADD_ERR (err, NULL_PTR);
        return 1;
    }

    return 0;
}

#define DEB printf("DEB %d\n", __LINE__)

bool is_valid_stack (const Stack *stack) {
    if (!stack)
        return 0;
    if (stack->size > stack->capacity)
        return 0;
    if (!stack->arr && stack->capacity)
        return 0;
    if (!stack->size_el && stack->capacity) // size_el can be 0 only if stack is empty
        return 0;
    if ( 
        // all are good or all are zero
        (stack->begin_canary != get_good_canary (stack) ||
        stack->end_canary != get_good_canary (stack))
        &&
        (stack->size_el || 
        stack->begin_canary || stack->end_canary)
    )
        return 0;

    return 1;
}

bool is_empty_stack (const Stack *stack, RetErr *err) {
    if (check_null (stack, err))
        return 0;
    if (!check_stack (stack, err))
        return 0;

    return !stack->size_el; // all other members are guaranteed to be 0
}


#undef malloc
#undef realloc





