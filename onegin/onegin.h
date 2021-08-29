#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// struct for saving beginning and len of string
// string can be not null-terminated
// useful for more effecient sorting by rhyme to access end of string
struct DoubleString {
    char *begin;
    int len;
};

// opens file and returns it on success
// if failed it prints error and return NULL
FILE *fopen_err (const char *name, const char *mode);

// reads strings from file file_in to array of char*
// number of read lines is put into *num_of_lines
DoubleString *read_text_file (FILE *file_in, int *num_of_lines);

// prints array of DoubleString of length len to file_out
void print_double_strings (DoubleString *strings, int len, FILE *file_out);

// returns 1 if string has any alnum character
// otherwise 0
int has_alnum (const char *str);

// compares two DoubleStrings in lexicographical order,
// ignoring leading not alnum characters 
int cmp_first_alnum_DS (const void *a, const void *b);

// compares two DoubleStrinds by last symbols in lexicographical order
int cmp_rhyme_DS (const void* a, const void* b);