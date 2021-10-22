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

void str_to_upper (char *str) {
    assert (str);

    while (*str) {
        *str = toupper (*str);
        str++;
    }
}


