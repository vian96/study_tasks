#ifndef MY_STACK_H
#define MY_STACK_H

#include "config.h"

#include <stdio.h>
#include <inttypes.h>

const size_t MIN_STACK_SIZE = 64;

// TODO do function change_capacity
struct Stack {
#ifdef CANARY_PROTECTION
    uint64_t begin_canary;
#endif

    size_t size;
    size_t capacity;
    size_t size_el;
    void *arr;

// TODO canary(code) instead of ifdef canary...

#ifdef HASH_DATA_PROTECTION
    uint64_t data_hash;
#endif // HASH_DATA_PROTECTION

#ifdef HASH_PROTECTION
    uint64_t hash;
#endif // HASH_PROTECTION

#ifdef CANARY_PROTECTION
    uint64_t end_canary;
#endif // HASH_PROTECTION
};

// TODO print errors in beatiful way
enum RetErr {
    // TODO add STACK_
    STACK_OK                = 0, 
    STACK_OVERFLOW          = 1, 
    STACK_UNDERFLOW         = 1 << 1,
    STACK_NO_MEMORY         = 1 << 2, 
    STACK_INVALID_STACK     = 1 << 3, 
    STACK_NULL_PTR          = 1 << 4,
    STACK_INVALID_ARG       = 1 << 5, 
    STACK_ERR_WRITING_FILE  = 1 << 6,
    STACK_ERR_OPEN_FILE     = 1 << 7
};

enum DumpMode {
    STACK_ADDR          = 1 << 1,
    STACK_MEMB          = 1 << 2,
    STACK_DATA_ADDR     = 1 << 3,
    STACK_DATA          = 1 << 4,
    STACK_EXPECTED      = 1 << 5,
    STACK_FORCE_DATA    = 1 << 6
};

const DumpMode MAX_DUMP = (DumpMode) ((1 << 6) - 1);

DumpMode add_modes (size_t num, ...);

DumpMode match_dump_mode(DumpMode mode, DumpMode match);

void set_log_file (const char *name = "stack_logs.txt", const char *mode = "a", RetErr *err = nullptr);

void close_log_file ();

void *realloc_ (void *ptr, const size_t len, const size_t size);

void print_stack_int (const void *elem, FILE *f_out, RetErr *err = nullptr);

void add_err (RetErr *err, RetErr value);

uint64_t calc_hash (const void *ptr, size_t size);

uint64_t calc_stack_hash (const Stack *stack);

uint64_t calc_stack_data_hash (const Stack *stack);

// constructor and desctructor
void stack_ctor (Stack *stack, size_t capacity, size_t size_el, RetErr *err = nullptr);

void stack_dtor (Stack *stack, RetErr *err = nullptr);

// element access functions
void stack_push (Stack *stack, const void *value, RetErr *err = nullptr);

const void *stack_pop (Stack *stack, RetErr *err = nullptr);

const void *stack_top (const Stack *stack, RetErr *err = nullptr);

// debug functions
void stack_dump (const Stack *stack, FILE *f_out, DumpMode mode, 
                    void (* print) (const void *elem, FILE *f_out, RetErr *err) = nullptr,
                    RetErr *err = nullptr);

// stack tests
bool check_stack (const Stack *stack, RetErr *err = nullptr, bool dump = 1);

bool check_null (const void *ptr, RetErr *err = nullptr);

bool is_valid_stack (const Stack *stack);

bool is_empty_stack (const Stack *stack, RetErr *err = nullptr);

#endif // MY_STACK_H
