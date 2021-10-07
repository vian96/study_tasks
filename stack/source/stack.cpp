#include "config.h"

#include "stack.h"

// canary protection utils

#ifdef CANARY_PROTECTION

const uint64_t GOOD_CANARY = ((uint64_t)1000 - 7) * 300/*$*/ * 1488 * 1337 * 666 * 228 * 107;

uint64_t get_good_canary (const void *ptr) {
    return GOOD_CANARY ^ (uint64_t) ptr;
}

#endif // CANARY PROTECTION

// to avoid misusing bad functions

void *realloc_ (void *ptr, const size_t len, const size_t size) {
    return realloc (ptr, len * size);
}

#define malloc() ((void) 0)
#define realloc() ((void) 0)

void print_stack_int (const void *elem, FILE *f_out, RetErr *err) {
    if (check_null ((void*) (elem && f_out), err))
        return;

    if (!elem || !f_out) {
        add_err (err, NULL_PTR);
        return;
    }

    fprintf (f_out, "%d ", *(int *) elem);
}

void add_err (RetErr *err, RetErr value) {
    err && (*err = (RetErr)((int)(*err) | (int)value));
}

DumpMode add_modes (size_t num, ...) {
    assert (num > 0);
    
    va_list valist;
    va_start (valist, num);

    // DumpMode var promotes to int so 
    // you should pass int to va_arg to get correct behaviour

    DumpMode res = (DumpMode) va_arg (valist, int);

    for (size_t i = 1; i < num; i++) 
        res = (DumpMode) ((int) res | (int) va_arg (valist, int));
    
    va_end (valist);

    return res;
}

DumpMode match_dump_mode(DumpMode mode, DumpMode match) {
    return (DumpMode) ((int) (mode) & (int) (match));
}

uint64_t calculate_hash (const void *ptr, size_t size) {
	uint64_t hash = 0;

	for (size_t i = 0; i < size; i++) {
        //printf ("DEB %d\n", i);
		hash += (uint8_t) (((char *) ptr) [i]);
		hash -= (hash << 13) | (hash >> 19);
	}

	return hash;
}

uint64_t calculate_stack_hash (const Stack *stack) {
    return calculate_hash (stack, (size_t)(&(stack->hash)) - (size_t)stack);
}

// constructor and destructor

void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err) {
    if (!is_empty_stack (stack, err))
        return;
        
#ifdef CANARY_PROTECTION
    stack->begin_canary = get_good_canary (&stack->begin_canary);
    stack->end_canary = get_good_canary (&stack->end_canary);
#endif // CANARY PROTECTION

    stack->capacity = capacity;
    stack->size_el = size_el;

    if (capacity) {
        if (capacity < MIN_STACK_SIZE)
            capacity = MIN_STACK_SIZE;
        
        stack->arr = calloc (capacity, size_el);
        stack->capacity = capacity;

        if (!stack->arr)
            add_err (err, NO_MEMORY);
    }
    
#ifdef HASH_PROTECTION
    stack->hash = calculate_stack_hash (stack);
#endif // HASH_PROTECTION
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

#ifdef CANARY_PROTECTION
    stack->begin_canary = 0;
    stack->end_canary = 0;
#endif // CANARY PROTECTION

#ifdef HASH_PROTECTION
    stack->hash = 0;
#endif // HASH_PROTECTION
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
        void *temp = realloc_ (stack->arr, stack->capacity * 2, stack->size_el);
        
        if (!temp) { 
            add_err (err, NO_MEMORY);
            
            return;
        }

        stack->capacity *= 2;
        stack->arr = temp;
    }

    memcpy (
        (char *)stack->arr + (stack->size) * (stack->size_el), 
        value, stack->size_el
    );
    
    stack->size++;

#ifdef HASH_PROTECTION
    stack->hash = calculate_stack_hash (stack);
#endif // HASH_PROTECTION
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

    --(stack->size);

#ifdef HASH_PROTECTION
    stack->hash = calculate_stack_hash (stack);
#endif // HASH_PROTECTION

    return (char *)(stack->arr) + (stack->size) * stack->size_el;
}

void *stack_top (const Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return nullptr;

    if (!stack->size) {
        add_err (err, UNDERFLOW);

        return nullptr;
    }

    return (char *)(stack->arr) + (stack->size - 1) * stack->size_el;
}

// debug functions

void stack_dump (const Stack *stack, FILE *f_out, DumpMode mode, 
                    void (* print) (const void *elem, FILE *f_out, RetErr *err),
                    RetErr *err) {
    // TODO should i split it into smaller functions?
    // TODO printf line, name, function and so on..
    if (check_null ((void*) (f_out && print), err)) {
        return;
    }
    if (ferror(f_out)) {
        add_err (err, INVALID_ARG);
        return;
    }
    
    fprintf (f_out, "\n-------------STACK DUMP---------------\n");

    if (!stack) {
        fprintf (
            f_out,
            "ERROR: stack address is NULL!!!!!\n"
            "--------------------------------------\n"
        );
        add_err (err, NULL_PTR);
    }

    bool is_stack_ok = check_stack (stack, err);
    
    if (!is_stack_ok) {
        // TODO add message why there is an error
        fprintf (f_out, "Stack status: BAD\n");

        return;
    } 
    
    fprintf (f_out, "Stack status: OK\n");

    if (match_dump_mode (mode, STACK_ADDR)) {
        fprintf (f_out, "Address: %p \n", stack);
    }

    if (match_dump_mode (mode, STACK_MEMB)) {
        fprintf (
            f_out,
            "Stack members:\n"
            "    Size: %d\n"
            "    Capacity: %d\n"
            "    Size of element: %d\n",
            stack->size, stack->capacity, stack->size_el
        );
    }
    
    if (match_dump_mode (mode, STACK_DATA_ADDR)) {
        fprintf (f_out, "Pointer to data: %p\n", stack->arr);
    }

    if (match_dump_mode (mode, STACK_DATA)) {
        if (print) {
            RetErr t_err = OK;
            
            fprintf (f_out, "Data from stack: \n");
            for (size_t i = 0; i < stack->size; i++) {
                print ((char*) stack->arr + i * stack->size_el, f_out, &t_err);
            
                if (t_err) {
                    add_err (err, t_err);
                    return;
                }
            }
            fputc ('\n', f_out);
        }
        else {
            fprintf (f_out, "Raw data from stack: \n");

            for (size_t i = 0; i < stack->size * stack->size_el; i++)
                fprintf (f_out, "%X ", ((char *) stack->arr)[i] & 0xff);
                
            fputc ('\n', f_out);
        }
    }

    fprintf (
        f_out,
        "---------END OF STACK DUMP------------\n\n"
    );

    if (ferror (f_out))
        add_err (err, ERR_WRITING_FILE);
}

// stack tests
bool check_stack (const Stack *stack, RetErr *err = nullptr) {
    if (!stack) {
        add_err (err, NULL_PTR);
        return 0;
    }
    if (!is_valid_stack (stack)) {
        add_err (err, INVALID_STACK);
        return 0;
    }

    return 1;
}

bool check_null (const void *ptr, RetErr *err) {
    if (!ptr) {
        add_err (err, NULL_PTR);
        return 1;
    }

    return 0;
}

bool is_valid_stack (const Stack *stack) {
    if (!stack)
        return 0;
    if (stack->size > stack->capacity)
        return 0;
    if (!stack->arr && stack->capacity)
        return 0;
    if (!stack->size_el && stack->capacity) // size_el can be 0 only if stack is empty
        return 0;

#ifdef CANARY_PROTECTION
    if ( 
        // all are good or all are zero
        (stack->begin_canary != get_good_canary (&stack->begin_canary) ||
        stack->end_canary != get_good_canary (&stack->end_canary))
        &&
        (stack->size_el || 
        stack->begin_canary || stack->end_canary)
    )
        return 0;
#endif // CANARY PROTECTION

#ifdef HASH_PROTECTION
    if (
        // all are good or all are zero
        stack->hash != calculate_stack_hash (stack) 
        &&
        (stack->size_el || stack->hash)
    )
        return 0;
#endif // HASH_PROTECTION

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





