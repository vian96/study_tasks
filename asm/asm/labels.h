#ifndef ASM_LABELS_H
#define ASM_LABELS_H

#include "../config.h"

const int LABEL_LEN = 30;
const int LABEL_NUM = 100;

struct Label {
    char name[LABEL_LEN];
    int value;
};

// returns -1 on failure
int get_label_val (const char *str);

// return 1 on succes, -1 on failure
int set_label_val (const char *str, int val);

Label *get_label (const char *str);

#endif // ASM_LABELS_H
