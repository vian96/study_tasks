#include "onegin.h"

void onegin () {
    FILE *f_in = fopen_err ("poem.txt", "r");
    if (!f_in)
        return;

    FILE *f_out = fopen ("output.txt", "a");
    if (!f_out) 
        return;

    int filled = 0;
    char **strings = read_text_file (f_in, &filled);

    char **orig = (char **) calloc (filled, sizeof (*strings));
    memcpy (orig, strings, filled * sizeof (*strings));

    fputs ("/////////////////////////////////////////////////////\n"
           "Sorted by first symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, filled, sizeof (*strings), cmp_first_alnum);
    fprint_strings (strings, filled, f_out);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Sorted by last symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, filled, sizeof (*strings), cmp_rhyme);
    fprint_strings (strings, filled, f_out);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Original text\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    fprint_strings (orig, filled, f_out);

    // freeing memory and closing files

    for (int k = 0; k < filled; k++)
        free (strings[k]);
    free (strings);
    free (orig);

    fclose (f_in);
    fclose (f_out);
}

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

    int num_strings = 300;
    char **strings = (char **) calloc (num_strings, sizeof (char*));

    for (int i = 0; i < num_strings; i++)
        strings[i] = (char *) calloc (MAX_STR_LEN, sizeof (char));

    while (1) {
        if (fgets (strings[*num_of_lines], MAX_STR_LEN, file_in) == NULL) {
            if (feof (file_in))
                break;
            
            if (ferror (file_in)) {
                printf ("ERROR: reading from file failed!\n");
                return 0;
            }
        }

        if (has_alnum (strings[*num_of_lines]))
            (*num_of_lines)++;

        if (*num_of_lines == num_strings) {
            num_strings *= 2;
            strings = (char **) realloc (strings, num_strings * sizeof (*strings));

            for (int i = *num_of_lines; i < num_strings; i++)
                strings[i] = (char *) calloc (MAX_STR_LEN, sizeof (char));
        }
    }

    for (int i = *num_of_lines; i < num_strings; i++)
        free (strings[i]);

    strings = (char **) realloc (strings, *num_of_lines * sizeof (char *));

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

