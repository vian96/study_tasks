#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

// struct for saving beginning and len of string
// string can be not null-terminated
// useful for more effecient sorting by rhyme to access end of string
struct StringRef {
    char *begin;
    int len;
};

struct FileText {
    StringRef *strings;
    int count;
};

// opens file and returns it on success
// if failed it prints error and return NULL
FILE *fopen_err (const char *name, const char *mode);

// reads strings from file file_in to array of char*
// number of read lines is put into *num_of_lines
// returns array of StringRef pointing to lines of text
FileText read_text_file (FILE *file_in);

// clears FileText and frees memory
// if order of strings is changed, pass orig to 
// begin of the first string from original
void free_file_text (FileText *text, char *orig = nullptr);

// reads symbols from file 
// number of read symbols is put into len
char *read_symbols_file (FILE *file_in, int *len);

// returns size of file 
// be careful when opening files in text mode,
// it may return too much symbols, see:
// neowin.net/forum/topic/555097-cc-fseek-ftell-with-text-files/?do=findComment&comment=588494916
int size_of_file (FILE *file);

// prints array of StringRef of length len to file_out
void fprint_string_ref (StringRef *strings, int len, 
                            FILE *file_out, int ignore_empty);
                            
// checks if str has alphanum characters
int has_alnum_SR (const StringRef str);
