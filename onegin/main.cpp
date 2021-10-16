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

    FileText text = read_text_file (f_in);
    char *orig = text.strings->begin;
    
    fclose (f_in);  

    FILE *f_out = fopen ("output.txt", "a");
    if (!f_out) 
        return;

    fputs ("/////////////////////////////////////////////////////\n"
           "Sorted by first symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (text.strings, text.count, sizeof (StringRef), cmp_first_alnum_SR);
    fprint_string_ref (text.strings, text.count, f_out, 1);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Sorted by last symbols\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    qsort (text.strings, text.count, sizeof (StringRef), cmp_rhyme_SR);
    fprint_string_ref (text.strings, text.count, f_out, 1);

    fputs ("\n\n/////////////////////////////////////////////////////\n"
           "Original text\n"
           "/////////////////////////////////////////////////////\n\n",
           f_out
    );
    
    for (int i = 0; i < text.count; i++) {
        fputs (text.strings[i].begin, f_out);
        fputc ('\n', f_out);
    }

    fclose (f_out);

    free_file_text (&text, orig);

    printf ("DONE!");
}
