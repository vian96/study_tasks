// Meta language for commands and define of DEF_CMD_ for cpu
// you should include "unmeta.h" after using this defines

#define PUSH(val) {                     \
    int val__ = (val);                  \
    stack_push (&cpu->stack, &val__);   \
    DEB ("Pushed %d\n", val__);         \
}

// TODO function for POP, it is ugly with macro and doesn't work properly
#define POP (*safe_pop (cpu))

#define ARG (*get_arg (cpu->bin + cpu->ip, cpu))

// TODO know why [num] doesn't work in this define
// and solve problem with arg_size
#define JUMP(pos) {                                 \
    int pos__ = (pos);                              \
    cpu->ip = pos__ - ARG_SIZE * cmd_args[cmd];     \
    DEB ("Jumping to %d\n", pos__);                 \
}

#define DEF_CMD_(num, name, args, code)                     \
    case CMD_##name:                                        \
        DEB ("Doing " #name " at pos %d\n", cpu->ip);       \
        cpu->ip += CMD_SIZE;                                \
                                                            \
        code;                                               \
                                                            \
        /* TODO change next line for jmp command */         \
        cpu->ip += cmd_args[num] * ARG_SIZE;                \
                                                            \
        break;                                  
// end of define DEF_CMD_
