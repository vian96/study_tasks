#include "commands.h"

#include "../onegin/file_utils.h"
#include "../stack/source/stack.h"

#include <stdio.h>

#include <conio.h>

#define NDEBUG

#ifndef NDEBUG

#define DEB(...) printf (__VA_ARGS__);
#define wait //_getch();

#else // debug

#define DEB(...) 
#define wait 

#endif // not debug

const int num_regs = 10;

struct Cpu {
    const char *bin;
    int ip;

    Stack stack;
    int regs [num_regs]; // {} are not empty for compatability with C
};

// TODO should i leave it global?
Cpu cpu = {};

void cpu_ctor ();

void cpu_dtor ();

// TODO divide in different files

int *get_arg (const char *bin);

void clear_input_buffer ();

int input_int ();

bool check_file_data (const char *bin);

// TODO unused function, remove it
uint64_t reverse_bytes (uint64_t bytes);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf (
            "ERROR: wrong number of arguments\n"
            "You shold call \"cpu *name*.boac\"!\n"
        );
        return 0;
    }

    const char *in_name = argv[1];

    if (strlen (in_name) < 6 || strcmp (in_name + strlen (in_name) - 5, ".boac") != 0) {
        printf (
            "ERROR: wrong name of input file\n"
            "Input file should have name \"*name*.boac\" where len(name) > 0\n"
        );
    } 

    FILE *f_in = fopen_err (in_name, "rb");
    if (!f_in)
        return 0;

    cpu_ctor ();
    DEB ("Initialized cpu..\n");

    int file_len = 0;
    cpu.bin = read_symbols_file (f_in, &file_len);

    fclose (f_in);

    if (sizeof (ASM_SIGN) + sizeof (ASM_VER) >= file_len) {
        printf ("ERROR: file is corrupted, it is too small\n");
        cpu_dtor ();
        
        return 0;
    }

    if (!check_file_data (cpu.bin)) {
        cpu_dtor ();

        return 0;
    }

    cpu.ip += sizeof (ASM_VER) + sizeof (ASM_SIGN);

    DEB ("Starting executing..\n");
    
#define PUSH(val) {int VAL__ = (val); stack_push (&cpu.stack, &VAL__);}
#define POP (*(int*) stack_pop (&cpu.stack))
#define ARG (*get_arg (cpu.bin + cpu.ip))

    while (cpu.ip < file_len) {
        DEB ("while..\n");

#define DEF_CMD_(num, name, args, code)                 \
    case CMD_##name:                                    \
        DEB ("Doing " #name " at pos %d\n", cpu.ip);    \
        cpu.ip += CMD_SIZE;                             \
                                                        \
        code;                                           \
                                                        \
        cpu.ip += cmd_args[num] * ARG_SIZE;             \
                                                        \
        wait;                                           \
        break;                                  
// end of define DEF_CMD_

        switch (*(cpu.bin + cpu.ip)) {
            
        #include "commands.defs"

        default:
            printf (
                "ERROR: unknown command at pos %d with value %d, exiting cpu!\n", 
                cpu.ip, cpu.bin[cpu.ip]
            );
            cpu_dtor ();

            return 0;
        }
    }

#undef DEF_CMD_

#undef PUSH
#undef POP
#undef READ_ARG
#undef SET_ARG

    cpu_dtor ();

    printf ("DONE\n");
}

void cpu_ctor () {
    cpu.bin = nullptr;
    cpu.ip = 0;
    
    for (int i = 0; i < num_regs; i++)
        cpu.regs[i] = 0;
    
    cpu.stack = {};
    stack_ctor (&cpu.stack, 0, sizeof (int));
}

void cpu_dtor () {
    free ((char*) cpu.bin);

    cpu.bin = nullptr;
    cpu.ip = 0;
    
    for (int i = 0; i < num_regs; i++)
        cpu.regs[i] = 0;
    
    stack_dtor (&cpu.stack);
}

int *get_arg (const char *bin) {
    assert (bin);

    char type = *bin;
    bin++;
    int data = *(int*) bin;

    switch (type) {
    case ARG_INT:
        return (int*) bin;
    
    case ARG_REG:
        return cpu.regs + data;
    
    default:
        // TODO change assert to verify ...
        // program should not get here
        assert (("Unknown type of arg to read", 0));
        return 0;
    }
}

int input_int () {
    int x = 0;
    printf ("Input int: ");

    while (!scanf ("%d", &x)) {
        printf ("There is an error in your input, try again:\n");
        clear_input_buffer ();
    }

    return x;
}

// TODO unused function, remove it
uint64_t reverse_bytes (uint64_t bytes) {
    uint64_t aux = 0;
    uint8_t byte;

    for (int i = 0; i < 64; i += 8) {
        byte = (bytes >> i) & 0xff;
        aux |= byte << (64 - 8 - i);
    }
    return aux;
}

void clear_input_buffer () {
    // clears input buffer until the end of line

    while (getchar () != '\n') { ; }
}

bool check_file_data (const char *bin) {
    int asm_sign = 0;
    char asm_ver[8] = "";

    memcpy (&asm_sign, bin, sizeof (asm_sign));
    memcpy (&asm_ver, bin + sizeof (asm_sign), sizeof (asm_ver));
    

    if (asm_sign != ASM_SIGN) {
        printf ("ERROR: file is corrupted, signature is not okay\n");
        
        return 0;
    }

    if (strcmp (asm_ver, ASM_VER) != 0) {
        printf ("File version is not compatible with this cpu, expected: %s, got: %s\n", ASM_VER, asm_ver);
        
        return 0;
    }

    return 1;
}


