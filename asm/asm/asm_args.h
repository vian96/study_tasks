#ifndef ASM_ASM_ARGS_H
#define ASM_ASM_ARGS_H

#include "../config.h"
#include "../commands.h"
#include "../args.h"

int count_num_args (const char *cmd);

void set_int (char *dest, int val);

void set_arg (const char **str, char *out);

int get_type_arg (const char *str);

#endif // ASM_ASM_ARGS_H
