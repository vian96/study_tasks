#include "config.h"
#include "stack.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// canary protection utils

FILE *log_file = nullptr;

#if defined (CANARY_PROTECTION) || defined (CANARY_DATA_PROTECTION)

const uint64_t GOOD_CANARY = ((uint64_t) 1000 - 7) * 300/*$*/ * 1488 * 1337 * 666 * 228 * 107;

uint64_t get_good_canary (const void *ptr) {
    return GOOD_CANARY ^ (uint64_t) ptr;
}

void set_good_canary (uint64_t *ptr) {
    *ptr = get_good_canary (ptr);
}

bool check_canary (const uint64_t *ptr) {
    return *ptr == get_good_canary (ptr);
}

#endif // CANARY PROTECTION || CANARY_DATA_PROTECTION

// to avoid misusing bad functions

void *realloc_ (void *ptr, size_t len, size_t size) {
    return realloc (ptr, len * size);
}

#define malloc (YoU_ArE__UsInG_WrONG_funCTion___)
#define realloc (YoU_ArE__UsInG_WrONG_funCTion___)

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

void set_log_file (const char *name, const char *mode, RetErr *err) {
    if (log_file)
        fclose (log_file);
    
    log_file = fopen (name, mode);
    if (ferror (log_file))
        add_err (err, STACK_ERR_OPEN_FILE);
}

void close_log_file () {
    fclose (log_file);

    log_file = nullptr;
}


void print_stack_int (const void *elem, FILE *f_out, RetErr *err) {
    if (check_null ((void*) (elem && f_out), err))
        return;

    if (!elem || !f_out) {
        add_err (err, STACK_NULL_PTR);
        return;
    }

    fprintf (f_out, "%d ", *(int*) elem);
}

void add_err (RetErr *err, RetErr value) {
    err && (*err = (RetErr)((int)(*err) | (int)value));
}

#if defined (HASH_PROTECTION) || defined (HASH_DATA_PROTECTION)

uint64_t calc_hash (const void *ptr, size_t size) {
	uint64_t hash = 0;

	for (size_t i = 0; i < size; i++) {
		hash += (uint8_t) (((char*) ptr) [i]);
		hash -= (hash << 13) | (hash >> 19);
	}

	return hash;
}

uint64_t calc_stack_hash (const Stack *stack) {
    return calc_hash (stack, (size_t)(&stack->hash) - (size_t)stack);
}

uint64_t calc_stack_data_hash (const Stack *stack) {
    return calc_hash (stack->arr, stack->capacity * stack->size_el);
}

#endif // HASH_PROTECTION || HASH_DATA_PROTECTION

// constructor and destructor

void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err) {
    if (!is_empty_stack (stack, err))
        return;
        
#ifdef CANARY_PROTECTION
    set_good_canary (&stack->begin_canary);
    set_good_canary (&stack->end_canary);
#endif // CANARY PROTECTION

#ifdef HASH_DATA_PROTECTION
    stack->data_hash = 0;
#endif // HASH_DATA_PROTECTION

    stack->capacity = capacity;
    stack->size_el = size_el;

    if (capacity) {
        if (capacity < MIN_STACK_SIZE)
            capacity = MIN_STACK_SIZE;

#ifndef CANARY_DATA_PROTECTION 
        stack->arr = calloc (capacity * size_el, 1);
#else // not CANARY_DATA_PROTECTION
        stack->arr = calloc (capacity * size_el + 2 * sizeof (uint64_t), 1);
        set_good_canary ((uint64_t*) stack->arr);
        stack->arr = (char*) stack->arr + sizeof (uint64_t);

        set_good_canary ((uint64_t*) ((char*) stack->arr + capacity * size_el));
#endif // CANARY_DATA_PROTECTION

        stack->capacity = capacity;

        if (!stack->arr)
            add_err (err, STACK_NO_MEMORY);
    }
    
#ifdef HASH_PROTECTION
    stack->hash = calc_stack_hash (stack);
#endif // HASH_PROTECTION
}

void stack_dtor (Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return;
        
    // TODO what to do if there is allocated memory and there is error ?
    
#ifndef CANARY_DATA_PROTECTION
    free (stack->arr);
#else // not CANARY_DATA_PROTECTION
    free ((char*) stack->arr - sizeof (uint64_t));
#endif // CANARY_DATA_PROTECTION

    stack->arr = nullptr;
    stack->capacity = 0;
    stack->size = 0;
    stack->size_el = 0;

#ifdef CANARY_PROTECTION
    stack->begin_canary = 0;
    stack->end_canary = 0;
#endif // CANARY PROTECTION

#ifdef HASH_DATA_PROTECTION
    stack->data_hash = 0;
#endif // HASH_DATA_PROTECTION

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

    if (!stack->size_el) {
        add_err (err, STACK_INVALID_STACK);
        return;
    }

    if (!stack->capacity) {
        stack->capacity = MIN_STACK_SIZE;

#ifndef CANARY_DATA_PROTECTION
        stack->arr = (char *) calloc (stack->capacity, stack->size_el);
#else // not CANARY_DATA_PROTECTION
        stack->arr = (char*) calloc (stack->capacity * stack->size_el + 2 * sizeof (uint64_t), 1);
#endif // CANARY_DATA_PROTECTION
    }

    if (stack->size == stack->capacity) {
        char *temp = nullptr;

#ifndef CANARY_DATA_PROTECTION
        temp = (char *) realloc_ (stack->arr, stack->capacity * 2, stack->size_el);
#else // not CANARY_DATA_PROTECTION
        temp = (char*) realloc_ ((char*) stack->arr - sizeof (uint64_t), 
                                stack->capacity * 2 * stack->size_el + 2 * sizeof (uint64_t), 1);
#endif // CANARY_DATA_PROTECTION
        
        if (!temp) { 
            add_err (err, STACK_NO_MEMORY);
            return;
        }

#ifdef CANARY_DATA_PROTECTION
        temp += sizeof (uint64_t);

        set_good_canary ((uint64_t*) (temp + stack->capacity * 2 * stack->size_el));

        if (temp != stack->arr)
            set_good_canary ((uint64_t*) (temp - sizeof (uint64_t)));
#endif // CANARY_DATA_PROTECTION
 
        stack->capacity *= 2;
        stack->arr = temp;
    }

    memcpy (
        (char*) stack->arr + (stack->size) * (stack->size_el), 
        value, stack->size_el
    );
    
    stack->size++;

#ifdef HASH_DATA_PROTECTION
    stack->data_hash = calc_stack_data_hash (stack);
#endif // HASH_DATA_PROTECTION

#ifdef HASH_PROTECTION
    stack->hash = calc_stack_hash (stack);
#endif // HASH_PROTECTION
}

const void *stack_pop (Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return nullptr;

    if (!stack->size) {
        if (err)
            *err = STACK_UNDERFLOW;

        return nullptr;
    }

    if (stack->size <= stack->capacity / 4 && stack->capacity >= 2 * MIN_STACK_SIZE) {
        stack->capacity = stack->capacity / 2;

#ifndef CANARY_DATA_PROTECTION
        stack->arr = realloc_ (stack->arr, stack->capacity, stack->size_el);
#else // not CANARY_DATA_PROTECTION
        char *temp = (char*) realloc_ ((char*) stack->arr - sizeof (uint64_t), 
                                stack->capacity * stack->size_el + 2 * sizeof (uint64_t), 1);
        temp += sizeof (uint64_t);

        set_good_canary ((uint64_t*) (temp + stack->capacity * stack->size_el));
        stack->arr = temp;
#endif // CANARY_DATA_PROTECTION
        
#ifdef HASH_DATA_PROTECTION
        stack->data_hash = calc_stack_data_hash (stack);
#endif // HASH_DATA_PROTECTION
    }

    --stack->size;

#ifdef HASH_PROTECTION
    stack->hash = calc_stack_hash (stack);
#endif // HASH_PROTECTION

    return (char*) stack->arr + (stack->size) * stack->size_el;
}

const void *stack_top (const Stack *stack, RetErr *err) {
    if (!check_stack (stack, err))
        return nullptr;

    if (!stack->size) {
        add_err (err, STACK_UNDERFLOW);

        return nullptr;
    }

    return (char*) (stack->arr) + (stack->size - 1) * stack->size_el;
}

// debug functions

void stack_dump (const Stack *stack, FILE *f_out, DumpMode mode, 
                    void (* print) (const void *elem, FILE *f_out, RetErr *err),
                    RetErr *err) {
    // TODO should i split it into smaller functions?
    // TODO printf line, name, function and so on..
    if (ferror (f_out)) {
        add_err (err, STACK_INVALID_ARG);
        return;
    }

    if (!f_out)
        f_out = log_file;
    
    if (!f_out) {
        set_log_file ();
        f_out = log_file;
    }

    fprintf (f_out, "\n-------------STACK DUMP---------------\n");

    if (!stack) {
        fprintf (
            f_out,
            "ERROR: stack address is NULL!!!!!\n"
            "--------------------------------------\n"
        );
        add_err (err, STACK_NULL_PTR);
    }

    fprintf ( f_out, "Address of stack: %p\n", stack);

    bool is_stack_ok = check_stack (stack, err, 0);
    
    if (!is_stack_ok)
        // TODO add message why there is an error
        fprintf (f_out, "Stack status: BAD\n"); 
    else
        fprintf (f_out, "Stack status: OK\nStack is %sempty\n", 
                    is_empty_stack (stack) ? "" : "not ");

    if (match_dump_mode (mode, STACK_ADDR))
        fprintf (f_out, "Address: %p\n", stack);

    if (match_dump_mode (mode, STACK_MEMB)) {
        fprintf (
            f_out,
            "Stack members:\n"
            "    Size: %d\n"
            "    Capacity: %d\n"
            "    Size of element: %d\n",
            stack->size, stack->capacity, stack->size_el
        );
#ifdef CANARY_PROTECTION
        fprintf (
            f_out, 
            "    Canary: begin: %llu     end: %llu\n", 
            stack->begin_canary,
            stack->end_canary
        );
#endif // CANARY PROTECTION

#ifdef CANARY_DATA_PROTECTION
        if (is_stack_ok && stack->arr != nullptr)
            fprintf (
                f_out, 
                "    Data canary: begin: %llu     end: %llu\n", 
                *(uint64_t*) ((char*) stack->arr - sizeof (uint64_t)),
                *(uint64_t*) ((char*) stack->arr + stack->capacity * stack->size_el)
            );
        else 
            fprintf (f_out,  "   Data canary is not avaliable");
#endif // CANARY PROTECTION

#ifdef HASH_PROTECTION
        fprintf (f_out, "    Hash: %llu\n", stack->hash);
#endif // CANARY PROTECTION

#ifdef HASH_DATA_PROTECTION
        fprintf (f_out, "    Data hash: %llu\n", stack->data_hash);
#endif // CANARY PROTECTION
    }

    if (match_dump_mode (mode, STACK_EXPECTED)) {
        fprintf (f_out, "Expected values:\n");
        
#ifdef CANARY_PROTECTION
        fprintf (
            f_out, 
            "    Canary: begin: %llu     end: %llu\n", 
            get_good_canary (&stack->begin_canary),
            get_good_canary (&stack->end_canary)
        );
#endif // CANARY PROTECTION

#ifdef CANARY_DATA_PROTECTION
        if (is_stack_ok && stack->arr != nullptr)
            fprintf (
                f_out, 
                "    Data canary: begin: %llu     end: %llu\n", 
                get_good_canary ((uint64_t*) ((char*) stack->arr - sizeof (uint64_t))),
                get_good_canary ((uint64_t*) ((char*) stack->arr + 
                                                stack->capacity * stack->size_el))
            );
        else 
            fprintf (
                f_out, 
                "   Expected data canary is not avaliable"
            );
#endif // CANARY PROTECTION

#ifdef HASH_PROTECTION
        fprintf (f_out, "    Hash: %llu\n", calc_stack_hash (stack));
#endif // CANARY PROTECTION

#ifdef HASH_DATA_PROTECTION
        if (is_stack_ok && stack->arr != nullptr)
            fprintf (f_out, "    Data hash: %llu\n", calc_stack_data_hash (stack));
        else 
            fprintf (
                f_out, 
                "   Expected data hash is not avaliable"
            );
#endif // CANARY PROTECTION
    }
    
    if (match_dump_mode (mode, STACK_DATA_ADDR))
        fprintf (f_out, "Pointer to data: %p\n", stack->arr);

    if (match_dump_mode (mode, STACK_DATA) && is_stack_ok && stack->arr != NULL) {
        if (!stack->size) {
            fprintf (f_out, "No data in stack\n");
        }
        else if (print) {
            RetErr t_err = STACK_OK;

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
                fprintf (f_out, "%X ", ((char*) stack->arr)[i] & 0xff);
                
            fputc ('\n', f_out);
        }
    }

    fprintf (
        f_out,
        "---------END OF STACK DUMP------------\n\n"
    );

    if (ferror (f_out))
        add_err (err, STACK_ERR_WRITING_FILE);
}

// stack tests
bool check_stack (const Stack *stack, RetErr *err, bool dump) {
    if (!stack) {
        add_err (err, STACK_NULL_PTR);
        return 0;
    }
    if (!is_valid_stack (stack)) {
        add_err (err, STACK_INVALID_STACK);

        if (dump) 
            stack_dump (stack, nullptr, MAX_DUMP, nullptr, err);
            
        return 0;
    }

    return 1;
}

bool check_null (const void *ptr, RetErr *err) {
    if (!ptr) {
        add_err (err, STACK_NULL_PTR);
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
        (!check_canary (&stack->begin_canary) ||
         !check_canary (&stack->end_canary))
        &&
        (stack->size_el || 
         stack->begin_canary || stack->end_canary)
    )
        return 0;
#endif // CANARY PROTECTION

#ifdef CANARY_DATA_PROTECTION
    if (
        stack->arr 
        &&
        (!check_canary ((uint64_t*) ((char*) stack->arr - sizeof (uint64_t))) ||
         !check_canary ((uint64_t*) ((char*) stack->arr + stack->capacity * stack->size_el)))
    )
        return 0;
#endif // CANARY_DATA_PROTECTION

#ifdef HASH_PROTECTION
    if (
        // all are good or all are zero
        stack->hash != calc_stack_hash (stack) 
        &&
        (stack->size_el || stack->hash)
    )
        return 0;
#endif // HASH_PROTECTION

#ifdef HASH_DATA_PROTECTION
    if (
        // all are good or all are zero
        stack->data_hash != calc_stack_data_hash (stack) 
        &&
        (stack->size_el || stack->data_hash)
    )
        return 0;
#endif // HASH_DATA_PROTECTION

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





