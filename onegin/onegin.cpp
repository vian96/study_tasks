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
    
    // it may return too much symbols bc of text mode, see:
    // neowin.net/forum/topic/555097-cc-fseek-ftell-with-text-files/?do=findComment&comment=588494916
    fseek(file_in, 0, SEEK_END);
    int cnt_sym = ftell(file_in);
    fseek (file_in, 0, SEEK_SET);
    
    printf("Size: %d\n", cnt_sym);
    
    char *symbols = (char *) calloc (cnt_sym + 1, sizeof (char));

    printf("Read %d\n",fread (symbols, sizeof (char), cnt_sym, file_in));

    //puts (symbols);
    //printf ("|| %d %d %d %d|\n", symbols[cnt_sym-4], symbols[cnt_sym-5], symbols[cnt_sym-6], symbols[cnt_sym-8]);

    // counting all lines from file
    int cnt_lines = 1;

    for (int i = 0; i < cnt_sym; i++) 
        // TODO skip empty strings
        if (symbols[i] == '\n') 
            cnt_lines++;

    char **strings = (char **) calloc (cnt_lines, sizeof (char *));
    strings[0] = symbols;
    int line = 1;
    printf("Lines: %d\n", cnt_lines);

    for (int i = 0; i < cnt_sym; i++) 
        // TODO skip empty strings
        if (symbols[i] == '\n') {
            strings[line++] = symbols + i + 1; // +1 to avoid pointing to 0
            symbols[i] = 0;
        }
    
    *num_of_lines = cnt_lines;
    
    return strings;
}

void fprint_strings (char **strings, int len, FILE *file_out) {
    assert (strings);
    assert (file_out);
    assert (!ferror (file_out));
  
    for (int i = 0; i < len; i++) {
        fputs (strings[i], file_out);
        fputc ('\n', file_out);
    }
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
