#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

const int ASM_SIGN = 'CABO';
const char ASM_VER[8] = "0.3.5";
const int FILE_DATA_SIZE = sizeof (ASM_SIGN) + sizeof (ASM_VER);

const int SIZE_CMD = sizeof (char);

#define DEF_CMD_(num, name, args, code) CMD_##name = num,
enum AsmCmd {
    #include "commands.defs"

    NUM_CMD     // auto completes to the next number
};
#undef DEF_CMD_

#define DEF_CMD_(num, name, args, code) #name,
const char cmd_names[NUM_CMD][10] = {
    "NOT_A_CMD", 

    #include "commands.defs"
};
#undef DEF_CMD_

#define DEF_CMD_(num, name, args, code) args,
const int cmd_args[NUM_CMD] = {
    -1, 

    #include "commands.defs"
};
#undef DEF_CMD_

#endif // ASM_COMMANDS_H



