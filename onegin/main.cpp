#include <stdio.h>
#include <stdlib.h>

#include "onegin.h"

int main() {
    const char *input_file = "poem.txt";
    FILE *f_in = fopen (input_file, "r");
    if (ferror (f_in)) {
        printf ("ERROR: opening file \"%s\" failed!\n", input_file);
        return 0;
    }

    const char *output_file = "output.txt";
    FILE *f_out = fopen (output_file, "a");
    if (ferror (f_out)) {
        printf ("ERROR: opening file \"%s\" failed!\n", output_file);
        return 0;
    }

    int filled = 0;
    char **strings = read_text_file (f_in, &filled);

    qsort (strings, filled, sizeof (*strings), cmp_first_alnum);
    
    for (int k = 0; k < filled; k++)
        fputs (strings[k], f_out);

    // freeing memory and closing files

    for (int k = 0; k < filled; k++)
        free (strings[filled]);
    free (strings);

    fclose (f_in);
    fclose (f_out);

    return 0;
}
