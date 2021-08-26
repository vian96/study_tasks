#include "my_string.h"

// puts character in string *src with length *n at place k
// if length is not enough, string is reallocated
void put_char_dyn (char **src, char c, size_t *n, int k) {
    if (k >= *n) {
        *n *= 2;
        *src = (char *) realloc (*src, *n * sizeof (char));
    }
    (*src)[k] = c;
}

int my_puts (const char *str) {
    assert(str);

    putchar (*str);

    while (*++str) 
        if( putchar (*str) == EOF)
            return EOF;

    return putchar ('\n');
}

char *strchr (const char *str, int ch) {
    assert (str);

    while (*str && *str != ch) 
        str++;    
    if (*str == ch)
        return (char *) str;
    return NULL;
}

size_t strlen (const char *str) {
    assert (str);

    size_t len = 0;
    while (*str++)
        len++;
    return len;
}

char *strcpy (char *dest, const char *src) {
    assert (dest);
    assert (src);
    assert (dest != src);

    char *orig = dest;
    while (*dest++ = *src++) { ; }

    return orig;
}

char *strncpy (char *dest, const char *src, size_t count) {
    assert (dest);
    assert (src);
    assert (dest != src);

    char *orig = dest;
    count++;
    while (--count && (*dest++ = *src++)) { ; }

    if (count)
        while (--count > 0)
            *dest++ = 0;
        
    return orig;
}

char *strcat (char *dest, const char *src) {
    assert (dest);
    assert (src);
    assert (dest != src);

    char *orig = dest;
    if (dest)
        while (*++dest)
            { ; }

    while (*dest++ = *src++) { ; }

    return orig;
}

char *strncat (char *dest, const char *src, size_t count) {
    assert (dest);
    assert (src);
    assert (dest != src);

    char *orig = dest;
    if (dest)
        while (*++dest)
            { ; }
            
    count++;
    while (--count && (*dest++ = *src++)) { ; }

    if (count)
        while (--count > 0)
            *dest++ = 0;

    return orig;   
}

char *my_fgets (char *str, int count, FILE *stream) {
    assert (str);
    assert (stream);
    
    if (count < 1)
        return NULL;

    if (count == 1){
        str[0] = 0;
        return str;
    }
    
    char *orig = str;
    while (--count) {
        int c = fgetc (stream);
        if (c == EOF) {
            if (feof (stream))
                if (orig != str)
                    // end of file
                    return orig;
            // failure
            return NULL;
        }

        if (c == '\n') {
            *str++ = '\n';
            *str = 0;
            return str;
        }

        *str++ = c;
    }    
    
    *str = 0;

    return str;
}

char *strdup (const char *str1) {
    assert (str1);
    char *str2 = (char *) calloc (strlen (str1), sizeof (char));
    return strcpy (str2, str1);
}

ssize_t my_getline (char **lineptr, size_t *n, FILE *stream) {
    assert (stream);
    
    if (*lineptr == NULL) {
        *n = 2;
        *lineptr = (char*) calloc (*n, sizeof (char));
    }

    int filled = 0;

    while (1) {
        int c = fgetc (stream);
        //printf ("%d\n", c);
        if (c == EOF) {
            if (feof (stream)) {
                put_char_dyn (lineptr, EOF, n, filled);
                // end of file
                break;
            }

            // failure
            free (*lineptr);
            *n = 0;

            return -1;
        }

        if (c == '\n') {
            put_char_dyn (lineptr, '\n', n, filled);
            break;
        }

        put_char_dyn (lineptr, c, n, filled);
        filled++;
    }
    
    if (filled + 2 != *n) 
        *lineptr = (char *) realloc (*lineptr, (filled + 2) * sizeof (char));
    
    (*lineptr)[filled + 1] = 0;

    return filled + 1;
}
