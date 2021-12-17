#include "string_utils.h"

#include <ctype.h>
#include <assert.h>

bool is_empty_string (const char *str) {
    assert (str);

    skip_blank (&str);
    
    return !*str;
}

void skip_blank (const char **str) {
    assert (str);
    assert (*str);

    while (**str && isblank(**str))
        (*str)++;
}

void skip_not_blank (const char **str) {
    assert (str);
    assert (*str);

    while (**str && !isblank(**str))
        (*str)++;
}

void skip_alnum (const char **str) {
    assert (str);
    assert (*str);

    while (**str && isalnum(**str))
        (*str)++;
}

void skip_name (const char **str) {
    assert (str);
    assert (*str);

    while (**str && is_in_name(**str))
        (*str)++;
}

void str_to_upper (char *str) {
    assert (str);

    while (*str) {
        *str = toupper (*str);
        str++;
    }
}

bool is_in_name (int c) {
    return isalnum (c) || c == '_';
}

void prepare_file_text (FileText *code) {
    assert (code);
    assert (code->strings);

    for (size_t line = 0; line < code->count; line++) {
        char *str = code->strings[line].begin;
        
        while (*str) {
            // removes comments
            if (*str == ';') {
                *str = '\0';
                break;
            }
            str++;
        }
    }
}

bool is_equal_words (const char *a, const char *b) {
    while (toupper(*a) == toupper(*b) && isalnum(*a) && isalnum(*b) && *a && *b) {
        a++; 
        b++;
    }

    // they're equal if they both reach end of word
    if (!isalnum (*a) && !isalnum (*b))
        return 1;

    return 0;
}

