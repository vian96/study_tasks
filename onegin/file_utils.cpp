#include "file_utils.h"

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

FileText read_text_file (FILE *file_in){
    assert (file_in);
    assert (!ferror (file_in));

    FileText text = {};
    text.count = 0;
    
    int cnt_sym = 0;
    char *symbols = read_symbols_file (file_in, &cnt_sym);

    // counting all lines from file
    text.count = 1;

    for (int i = 0; i < cnt_sym; i++) 
        if (symbols[i] == '\n') 
            text.count++;

    text.strings = (StringRef *) calloc (text.count, sizeof (StringRef));
    text.strings->begin = symbols;
    int line = 0;
    
    for (int i = 0; i < cnt_sym; i++) 
        if (symbols[i] == '\n') {
            text.strings[line].len = (int) (symbols + i - text.strings[line].begin);
            line++;
            text.strings[line].begin = symbols + i + 1;
            symbols[i] = '\0';
        }

    text.strings[line].len = (int) (symbols + cnt_sym - text.strings[line].begin);

    return text;
}

void free_file_text (FileText *text, char *orig) {
    if (!orig)
        free (text->strings[0].begin);
    else
        free (orig);
    free (text->strings);
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
            fputc ('\n', file_out);
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
