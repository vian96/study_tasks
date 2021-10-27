#ifndef ONEGIN_ONEGIN_H
#define ONEGIN_ONEGIN_H

#include "onegin.h"

int cmp_first_alnum_SR (const void *a, const void *b) {
    assert (a);
    assert (b);

    const StringRef *str1 = (const StringRef *) a, 
               *str2 = (const StringRef *) b;
    
    assert (str1->begin);
    assert (str2->begin);

    int i1 = 0, i2 = 0;
    
    while (!isalnum (str1->begin[i1]) && i1 < str1->len)
        i1++;
    while (!isalnum (str2->begin[i2]) && i2 < str2->len)
        i2++;

    while (str1->begin[i1] == str2->begin[i2] && i1 < str1->len && i2 < str2->len) {
        i1++;
        i2++;
    }

    if (str1->begin[i1] != str2->begin[i2]) 
        return str1->begin[i1] - str2->begin[i2];
    
    if (i1 != str1->len)
        return 1;
    if (i2 != str2->len)
        return -1;

    return 0;
}

int cmp_rhyme_SR (const void* a, const void* b) {
    assert (a);
    assert (b);

    const StringRef *str1 = (const StringRef *) a, 
               *str2 = (const StringRef *) b;

    assert (str1->begin);
    assert (str2->begin);

    int i1 = str1->len, i2 = str2->len;
    
    while (!isalnum (str1->begin[i1]) && i1 > 0)
        i1--;
    while (!isalnum (str2->begin[i2]) && i2 > 0)
        i2--;

    while (str1->begin[i1] == str2->begin[i2] && i1 < str1->len && i2 < str2->len) {
        i1--;
        i2--;
    }

    if (str1->begin[i1] != str2->begin[i2]) 
        return str1->begin[i1] - str2->begin[i2];
    
    if (i1 != str1->len)
        return 1;
    if (i2 != str2->len)
        return -1;

    return 0;
}

#endif // ONEGIN_ONEGIN_H

