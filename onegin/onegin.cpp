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

StringRef *read_text_file (FILE *file_in, int *num_lines) {
    assert (file_in);
    assert (!ferror (file_in));
    assert (num_lines);

    *num_lines = 0;
    
    int cnt_sym = 0;
    char *symbols = read_symbols_file (file_in, &cnt_sym);

    // counting all lines from file
    *num_lines = 1;

    for (int i = 0; i < cnt_sym; i++) 
        if (symbols[i] == '\n') 
            (*num_lines)++;

    StringRef *strings = (StringRef *) calloc (*num_lines, sizeof (StringRef));
    strings->begin = symbols;
    int line = 0;
    
    for (int i = 0; i < cnt_sym; i++) 
        if (symbols[i] == '\n') {
            strings[line].len = (int) (symbols + i - strings[line].begin + 1);
            line++;
            strings[line].begin = symbols + i + 1;
        }

    strings[line].len = (int) (symbols + cnt_sym - strings[line].begin + 1);

    return strings;
}

char *read_symbols_file (FILE *file_in, int *len) {
    assert (file_in);
    assert (!ferror (file_in));
    assert (len);

    *len = size_of_file (file_in);

    char *symbols = (char *) calloc (*len + 1, sizeof (char));
    int new_len = fread (symbols, sizeof (char), *len, file_in);

    if (new_len != *len) {
        *len = new_len;
        symbols = (char *) realloc (symbols, *len + 1);
    }

    symbols[*len] = 0;

    return symbols;
}

int size_of_file (FILE *file) {
    assert (file);
    assert (!ferror (file));

    fseek (file, 0, SEEK_END);
    int size = ftell (file);
    
    fseek (file, 0, SEEK_SET);

    return size;
}

void fprint_string_ref (StringRef *strings, int len, 
                                FILE *file_out, int ignore_empty) {
    assert (strings);
    assert (file_out);
    assert (!ferror (file_out));

    for (int i = 0; i < len; i++) 
        if (!ignore_empty || has_alnum_SR (strings[i])) {
            assert (strings[i].begin);

            fwrite (strings[i].begin, sizeof (char), strings[i].len, file_out);
        }
}

int has_alnum_SR (const StringRef str) {
    assert (str.begin);

    int i = 0;
    while (i < str.len) {
        if (isalnum (str.begin[i]))
            return 1;

        i++;
    }

    return 0;
}

int cmp_first_alnum_SR (const void *a, const void *b) {
    assert (a);
    assert (b);

    const StringRef *str1 = (const StringRef *) a, 
               *str2 = (const StringRef *) b;
    
    assert (str1->begin);
    assert (str2->begin);

    int i1 = 0, i2 = 0;
    
    while (!isalnum (str1->begin[i1]) && i1 < str1->len)
        i1++;
    while (!isalnum (str2->begin[i2]) && i2 < str2->len)
        i2++;

    while (str1->begin[i1] == str2->begin[i2] && i1 < str1->len && i2 < str2->len) {
        i1++;
        i2++;
    }

    if (str1->begin[i1] != str2->begin[i2]) 
        return str1->begin[i1] - str2->begin[i2];
    
    if (i1 != str1->len)
        return 1;
    if (i2 != str2->len)
        return -1;

    return 0;
}

int cmp_rhyme_SR (const void* a, const void* b) {
    assert (a);
    assert (b);

    const StringRef *str1 = (const StringRef *) a, 
               *str2 = (const StringRef *) b;

    assert (str1->begin);
    assert (str2->begin);

    int i1 = str1->len, i2 = str2->len;
    
    while (!isalnum (str1->begin[i1]) && i1 > 0)
        i1--;
    while (!isalnum (str2->begin[i2]) && i2 > 0)
        i2--;

    while (str1->begin[i1] == str2->begin[i2] && i1 < str1->len && i2 < str2->len) {
        i1--;
        i2--;
    }

    if (str1->begin[i1] != str2->begin[i2]) 
        return str1->begin[i1] - str2->begin[i2];
    
    if (i1 != str1->len)
        return 1;
    if (i2 != str2->len)
        return -1;

    return 0;
}

