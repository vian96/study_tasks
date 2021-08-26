#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int my_puts (const char *str);

char *strchr (const char *str, int ch);

size_t strlen (const char *str);

char *strcpy (char *dest, const char *src);

char *strncpy (char *dest, const char *src, size_t count);

char *strcat (char *dest, const char *src);

char *strncat (char *dest, const char *src, size_t count);

char *my_fgets (char *str, int count, FILE *stream);

char *strdup (const char *str1);

ssize_t my_getline (char **lineptr, size_t *n, FILE *stream);
