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

    int num_of_lines = 0;
    DoubleString *strings = read_text_file (f_in, &num_of_lines);
    char *orig = strings->begin;
    
    fclose (f_in);  

    FILE *f_out = fopen ("output.txt", "a");
    if (!f_out) 
        return;

    fputs ("/////////////////////////////////////////////////////\n"
           "Sorted by first symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, num_of_lines, sizeof (DoubleString), cmp_first_alnum_DS);
    print_double_strings (strings, num_of_lines, f_out);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Sorted by last symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, num_of_lines, sizeof (DoubleString), cmp_rhyme_DS);
    print_double_strings (strings, num_of_lines, f_out);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Original text\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    fputs (orig, f_out);

    fclose (f_out);

    // freeing memory and closing files

    free (orig);
    free (strings);
}
