#include "stack.h"

void *realloc_ (void *ptr, size_t len, size_t size) {
    return realloc (ptr, len * size);
}

// to avoid misusing another function
#define malloc() (void)(0)
#define realloc() (void)(0)

bool is_valid_stack (Stack *stack) {
    return stack && stack->size <= stack->capacity && stack->size_el;
}

bool is_empty_stack (Stack *stack, RetErr *err) {
    // if stack is empty it will not be valid because size_el == 0
    if (!is_valid_stack(stack) && stack->size_el) {
        if (err)
            *err = INVALID_ARG;
        return false;
    }

    return !stack->arr && !stack->capacity && !stack->size && !stack->size_el;
}

void stack_dump (Stack *stack, FILE *f_out, void (* print) (const void *elem, FILE *f_out, RetErr *err), RetErr *err) {
    if (!is_valid_stack(stack) || !f_out || ferror(f_out) || !print) {
        if (err)
            *err = INVALID_ARG;
        return;
    }

    if (err)
        *err = OK;
        
    for (int i = 0; i < stack->size; i++) {
        print ((char*)stack->arr + i * stack->size_el, f_out, err);
        if (err && *err)
            return;
    }
    fputc ('\n', f_out);
}

void print_stack_int (const void *elem, FILE *f_out, RetErr *err) {
    if (!elem || !f_out) {
        if (err)
            *err = INVALID_ARG;
        return;
    }

    fprintf (f_out, "%d ", *(int *) elem);
}

void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err) {
    if (!is_empty_stack (stack) || !size_el) {
        if (err)
            *err = INVALID_ARG;
        return;
    }
    
    stack->capacity = capacity;
    stack->size_el = size_el;
    
    // TODO small string optimization

    if (capacity) {
        if (capacity < MIN_STACK_SIZE)
            capacity = MIN_STACK_SIZE;
        
        stack->arr = calloc (capacity, size_el);
        stack->capacity = capacity;

        if (!stack->arr && err)
            *err = NO_MEMORY;
    }
}

void stack_dtor (Stack *stack, RetErr *err) {
    if (!stack) {
        if (err)
            *err = INVALID_ARG;
        return;
    }

    free (stack->arr);

    stack->arr = nullptr;
    stack->capacity = 0;
    stack->size = 0;
    stack->size_el = 0;
}

void stack_push (Stack *stack, const void *value, RetErr *err) {
    if (!is_valid_stack(stack) || !value) {
        if (err)
            *err = INVALID_ARG;
        return;
    }

    if (!stack->capacity)
        stack->capacity = MIN_STACK_SIZE;

    if (stack->size == stack->capacity) {
        stack->capacity *= 2;
        void *temp = realloc_ (stack->arr, stack->capacity, stack->size_el);
        
        if (!temp) { 
            if (err)
                *err = NO_MEMORY;
            
            return;
        }

        stack->arr = temp;
    }

    memcpy ((char *)stack->arr + (stack->size++) * (stack->size_el), value, stack->size_el);
}

void *stack_pop (Stack *stack, RetErr *err) {
    if (!is_valid_stack(stack)) {
        if (err)
            *err = INVALID_ARG;
        return NULL;
    }

    if (!stack->size) {
        // TODO should i change if to (err && *err = ...)
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

void *stack_top (Stack *stack, RetErr *err) {
    if (!is_valid_stack(stack)) {
        if (err)
            *err = INVALID_ARG;
        return NULL;
    }

    if (!stack->size) {
        // TODO should i change if to (err && *err = ...)
        if (err)
            *err = UNDERFLOW;

        return nullptr;
    }

    return (char *)(stack->arr) + (stack->size - 1) * stack->size_el;
}

#undef malloc
#undef realloc
