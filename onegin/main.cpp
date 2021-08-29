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

    int num_lines = 0;
    StringRef *strings = read_text_file (f_in, &num_lines);
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
    
    qsort (strings, num_lines, sizeof (StringRef), cmp_first_alnum_SR);
    fprint_string_ref (strings, num_lines, f_out, 1);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Sorted by last symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (strings, num_lines, sizeof (StringRef), cmp_rhyme_SR);
    fprint_string_ref (strings, num_lines, f_out, 1);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Original text\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    fputs (orig, f_out);

    fclose (f_out);

    free (orig);
    free (strings);
}
