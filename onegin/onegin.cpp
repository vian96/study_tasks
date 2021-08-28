#include "onegin.h"

FILE *fopen_err (const char *name, const char *mode) {
    assert (name);
    assert (mode);
    
    FILE *file = fopen (name, mode);
    
    if (ferror (file)) {
        printf ("ERROR: opening file \"%s\" failed!\n", name);
        return NULL;
    }

    return file;
}

char **read_text_file (FILE *file_in, int *num_of_lines) {
    assert (file_in);
    assert (!ferror (file_in));
    assert (num_of_lines);

    *num_of_lines = 0;

    // counting all chars and lines from file
    int ch = 0;
    int cnt_sym = 0, cnt_lines = 1;
    while ((ch = fgetc (file_in)) != EOF) {
        cnt_sym++;

        // TODO skip empty strings
        if (ch == '\n')
            cnt_lines++;
    }

    // reading all characters 
    char *symbols = (char *) calloc (cnt_sym + cnt_lines, sizeof (char));
    char **strings = (char **) calloc (cnt_lines, sizeof (char *));

    strings[0] = symbols;
    int sym = 0, line = 1;

    rewind (file_in);

    while ((ch = fgetc (file_in)) != EOF) {
        symbols[sym++] = ch;

        if (ch == '\n') {
            symbols[sym++] = '\0';
            strings[line++] = symbols + sym;
        }
    }
    
    *num_of_lines = cnt_lines;

    return strings;
}

void fprint_strings (char **strings, int len, FILE *file_out) {
    assert (strings);
    assert (file_out);
    assert (!ferror (file_out));
  
    for (int i = 0; i < len; i++)
        fputs (strings[i], file_out);
}

int has_alnum (const char *str) {
    assert (str);

    while (*str)
        if (isalnum (*(str++)))
            return 1;

    return 0;
}

int cmp_first_alnum (const void *a, const void *b) {
    assert (a);
    assert (b);
    
    const char *str1 = *(const char **) a, 
        *str2 = *(const char **) b;

    while (!isalnum (*str1) && *str1)
        str1++;
    while (!isalnum (*str2) && *str2)
        str2++;

    return strcmp (str1, str2);
}

int cmp_rhyme (const void* a, const void* b) {
    assert (a);
    assert (b);

    const char *str1 = *(const char **) a, 
               *str2 = *(const char **) b;

    const char *orig1 = str1,
               *orig2 = str2;

    while (*str1)
        str1++;
    while (*str2)
        str2++;

    while (!isalnum (*str1) && str1 != orig1)
        str1--;
    while (!isalnum (*str2) && str2 != orig2)
        str2--;

    while (*str1 == *str2 && str1 != orig1 && str2 != orig2) {   
        str1--;
        str2--;
    }

    if (*str1 != *str2)
            return *str1 - *str2;

    if (str1 != orig1)
        return 1;
    if (str2 != orig2)
        return -1;

    return 0;
}
