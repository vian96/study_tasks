#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "file_utils.h"

// compares two StringRefs in lexicographical order,
// ignoring leading not alnum characters 
int cmp_first_alnum_SR (const void *a, const void *b);

// compares two StringRefs by last symbols in lexicographical order
int cmp_rhyme_SR (const void* a, const void* b);