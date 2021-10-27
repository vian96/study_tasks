#include "../config.h"
#include "labels.h"
#include "string_utils.h"

#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// returns -1 on failure
int get_label_val (const char *str) {
    assert (str);

    Label *t = get_label (str);
    if (!t)
        return -1;
    
    return t->value;
}

// return 1 on succes, -1 on failure
int set_label_val (const char *str, int val) {
    assert (str);

    Label *t = get_label (str);
    if (!t)
        return -1;
    
    t->value = val;
    
    return 1;
}

Label *get_label (const char *str) {
    assert (str);

    static Label labels[LABEL_NUM] = {};
    char name[LABEL_LEN] = {}; 

    for (int i = 0; i < LABEL_LEN && str[i] && is_in_name (str[i]); i++) 
        name[i] = str[i];
    
    if (name[LABEL_LEN - 1]) {
        printf ("Syntax error: label name is too long, max is %d\n", LABEL_LEN);
        return nullptr;
    }

    if (!name[0]) {
        printf ("Syntax error: no label name\n");
        return nullptr;
    }

    int cnt = 0;
    for (cnt = 0; labels[cnt].name[0] && cnt < LABEL_NUM; cnt++)
        if (strcmp (labels[cnt].name, name) == 0) {
            DEB (
                "Found label at pos %d with name %s and val %d\n", 
                cnt, name, labels[cnt].value
            );
            
            return labels + cnt;
        }

    if (cnt == LABEL_NUM) {
        printf ("Compilation error: too much labels in code\n");
        return nullptr;
    }

    strcpy (labels[cnt].name, name);
    DEB (
        "Added label at pos %d with name %s and val %d\n", 
        cnt, name, labels[cnt].value
    );

    return labels + cnt;
}
