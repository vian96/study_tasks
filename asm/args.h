#ifndef ASM_ARGS_H
#define ASM_ARGS_H

const int ARG_SIZE = sizeof (int) + sizeof (char); 

enum ArgType {
    ARG_INT      = 1,
    ARG_REG      = 2,
    ARG_RAM      = 3,
    ARG_REG_RAM  = 4, // acces memory by address from reg
    ARG_LABEL    = 5
}; 

#endif // ASM_ARGS_H
