#include "onegin.h"

int has_alnum (const char *str) {
    while (*str)
        if (isalnum (*(str++)))
            return 1;
    return 0;
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
        char *res = fgets (strings[*num_of_lines], MAX_STR_LEN, file_in);

        if (res == NULL) {
            if (feof (file_in))
                break;
            
            if (ferror (file_in)) {
                printf ("ERROR: reading from file failed!\n");
                return 0;
            }
            
            continue;
        }

        if (has_alnum (strings[*num_of_lines]))
            (*num_of_lines)++;

        if (*num_of_lines == num_strings) {
            num_strings *= 2;
            strings = (char **) realloc (strings, num_strings * sizeof (*strings));

            for (int i = num_strings / 2; i < num_strings; i++)
                strings[i] = (char *) calloc (MAX_STR_LEN, sizeof (char));
        }
    }

    for (int i = *num_of_lines; i < num_strings; i++)
        free (strings[i]);
    strings = (char **) realloc (strings, *num_of_lines * sizeof (char *));

    return strings;
}

int cmp_first_alnum (const void *a, const void *b) {
    const char *str1 = *(const char **) a, 
        *str2 = *(const char **) b;

    while (!isalnum (*str1))
        str1++;
    while (!isalnum (*str2))
        str2++;

    //printf("%s %s", str1, str2);
    return strcmp (str1, str2);
}
