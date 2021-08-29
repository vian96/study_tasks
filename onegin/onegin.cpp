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

DoubleString *read_text_file (FILE *file_in, int *num_of_lines) {
    assert (file_in);
    assert (!ferror (file_in));
    assert (num_of_lines);

    *num_of_lines = 0;
    
    // it may return too much symbols bc of text mode, see:
    // neowin.net/forum/topic/555097-cc-fseek-ftell-with-text-files/?do=findComment&comment=588494916
    fseek(file_in, 0, SEEK_END);
    int cnt_sym = ftell(file_in);
    fseek (file_in, 0, SEEK_SET);
    char *symbols = (char *) calloc (cnt_sym + 1, sizeof (char));
    cnt_sym = fread (symbols, sizeof (char), cnt_sym, file_in);
    symbols[cnt_sym] = 0;

    // counting all lines from file
    int cnt_lines = 1;

    for (int i = 0; i < cnt_sym; i++) 
        // TODO skip empty strings
        if (symbols[i] == '\n') 
            cnt_lines++;

    DoubleString *strings = (DoubleString *) calloc (cnt_lines, sizeof (DoubleString));
    strings->begin = symbols;
    int line = 0;
    
    for (int i = 0; i < cnt_sym; i++) 
        // TODO skip empty strings
        if (symbols[i] == '\n') {
            strings[line].len = (int) (symbols + i - strings[line].begin + 1);
            line++;
            strings[line].begin = symbols + i + 1;
        }

    strings[line].len = (int) (symbols + cnt_sym - strings[line].begin + 1);

    *num_of_lines = cnt_lines;
    
    return strings;
}

void print_double_strings (DoubleString *strings, int len, FILE *file_out) {
    assert (strings);
    assert (file_out);
    assert (!ferror (file_out));

    for (int i = 0; i < len; i++) 
        fwrite (strings[i].begin, sizeof (char), strings[i].len, file_out);
}

int has_alnum (const char *str) {
    assert (str);

    while (*str)
        if (isalnum (*(str++)))
            return 1;

    return 0;
}

int cmp_first_alnum_DS (const void *a, const void *b) {
    assert (a);
    assert (b);

    const DoubleString *str1 = (const DoubleString *) a, 
               *str2 = (const DoubleString *) b;

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

int cmp_rhyme_DS (const void* a, const void* b) {
    assert (a);
    assert (b);

    const DoubleString *str1 = (const DoubleString *) a, 
               *str2 = (const DoubleString *) b;

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

