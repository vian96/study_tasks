#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

#include <string.h>
#include <ctype.h>

const int ASM_SIGN = 'CABO';

const  char ASM_VER[8] = "0.0.1";

struct FileData {
    int signature;
    long long version;
};

enum AsmCmd {
    CMD_PUSH    = 1,
    CMD_IN      = 2,
    CMD_MUL     = 3,
    CMD_OUT     = 4,
    CMD_ADD     = 5,
    NUM_CMD     // auto completes to the next number
};

const char cmd_names[NUM_CMD][10] = {"NOT_A_CMD", "push", "in", "mul", "out", "add"};

const int cmd_args[NUM_CMD] = {-1, 1, 0, 0, 0, 0};

bool is_equal_words (const char *a, const char *b) {
    while (*a == *b && !isblank(*a) && !isblank(*b) && *a && *b) {
        a++; 
        b++;
    }

    // they're equal or one of them is '\0' the other one is ' '
    if (*a == *b || (*a * *b == 0 && *a + *b == ' '))
        return 1;

    return 0;
}

AsmCmd get_cmd (const char *str) {
    for (int i = 0; i < NUM_CMD; i++)
        if (is_equal_words (str, cmd_names[i]) == 1)
            return (AsmCmd) i;

    return (AsmCmd) -1;
}

#endif // ASM_COMMANDS_H



