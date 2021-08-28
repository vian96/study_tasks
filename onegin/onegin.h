#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// max length of strings read from file
const int MAX_STR_LEN = 300;

// opens file and returns it on success
// if failed it prints error and return NULL
FILE *fopen_err (const char *name, const char *mode);

// reads strings from file file_in to array of char*
// number of read lines is put into *num_of_lines
char **read_text_file (FILE *file_in, int *num_of_lines);

// prints array of char* of length len to file file_out
void fprint_strings (char **strings, int len, FILE *file_out);

// returns 1 if string has any alnum character
// otherwise 0
int has_alnum (const char *str);

// compares two strings in lexicographical order,
// ignoring leading not alnum characters 
int cmp_first_alnum (const void *a, const void *b);

// compares two strings by last symbols in lexicographical order
int cmp_rhyme (const void* a, const void* b);

