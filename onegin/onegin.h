#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

const int MAX_STR_LEN = 300;

void onegin ();

int has_alnum (const char *str);

char **read_text_file (FILE *file_in, int *num_of_lines);

int cmp_first_alnum (const void *a, const void *b);

int cmp_rhyme (const void* a, const void* b);

void fprint_strings (char **strings, int len, FILE *file_out);

FILE *fopen_err (const char *name, const char *mode);