#include <stdio.h>
#include <stdlib.h>

#include "onegin.h"

void onegin ();

int main() {
    onegin ();

    return 0;
}

void onegin () {
    // TODO split function into smaller ones

    FILE *f_in = fopen_err ("poem.txt", "r");
    if (!f_in)
        return;

    FILE *f_out = fopen ("output.txt", "a");
    if (!f_out) 
        return;

    int num_of_lines = 0;
    char **strings = read_text_file (f_in, &num_of_lines);

    char *orig = *strings;

    fputs ("/////////////////////////////////////////////////////\n"
           "Sorted by first symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, num_of_lines, sizeof (*strings), cmp_first_alnum);
    fprint_strings (strings, num_of_lines, f_out);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Sorted by last symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, num_of_lines, sizeof (*strings), cmp_rhyme);
    fprint_strings (strings, num_of_lines, f_out);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Original text\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    // TODO function for printing ?
    int sym_ind = 0;
    for (int line = 0; line < num_of_lines; line++) {
        while (orig[sym_ind])
            fputc (orig[sym_ind++], f_out);
        sym_ind++;
    }

    // freeing memory and closing files

    free (orig);
    free (strings);

    fclose (f_in);
    fclose (f_out);
}
