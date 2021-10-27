#include "../config.h"
#include "../commands.h"
#include "../args.h"
#include "cpu.h"

#include "../../onegin/file_utils.h"
#include "../../stack/source/stack.h"

#include <windows.h>

#include <stdio.h>
#include <math.h>

void execute_cpu (Cpu *cpu);

int *get_arg (const char *bin, Cpu *cpu);

void clear_input_buffer ();

int input_int ();

bool check_file_data (const char *bin);

int *safe_pop (Cpu *cpu);

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
        return 0;
    } 

    DEB ("Reading file...\n");

    FILE *f_in = fopen_err (in_name, "rb");
    if (!f_in)
        return 0;

    Cpu cpu = {};
    cpu_ctor (&cpu);
    DEB ("Initialized cpu..\n");

    cpu.bin = read_symbols_file (f_in, &cpu.len);
    fclose (f_in);
    DEB ("Length of file: %d\n", cpu.len);

    if (sizeof (ASM_SIGN) + sizeof (ASM_VER) >= cpu.len) {
        printf ("ERROR: file is corrupted, it is too small\n");
        cpu_dtor (&cpu);
        
        return 0;
    }

    if (!check_file_data (cpu.bin)) {
        cpu_dtor (&cpu);

        return 0;
    }

    cpu.ip += sizeof (ASM_VER) + sizeof (ASM_SIGN);
    DEB ("Starting executing..\n");
    execute_cpu (&cpu);

    cpu_dtor (&cpu);
    printf ("DONE\n");

    return 0;
}

void cpu_ctor (Cpu *cpu) {
    cpu->bin = nullptr;
    cpu->ip = 0;
    cpu->len = 0;

    // TODO memset
    for (int i = 0; i < NUM_REGS; i++)
        cpu->regs[i] = 0;
    
    for (int i = 0; i < NUM_RAM; i++)
        cpu->ram[i] = 0;

    cpu->stack = {};
    stack_ctor (&cpu->stack, 0, sizeof (int));
}

void cpu_dtor (Cpu *cpu) {
    free ((char*) cpu->bin);

    cpu->bin = nullptr;
    cpu->ip = 0;
    cpu->len = 0;
    
    for (int i = 0; i < NUM_REGS; i++)
        cpu->regs[i] = 0;

    for (int i = 0; i < NUM_RAM; i++)
        cpu->ram[i] = 0;
    
    stack_dtor (&cpu->stack);
}

void execute_cpu (Cpu *cpu) {
    assert (cpu);
    assert (cpu->len);
    assert (cpu->bin);

#include "meta.h"

    while (cpu->ip < cpu->len) {
        DEB ("while..\n");

        int cmd = cpu->bin[cpu->ip];
        switch (cmd) {
            
        #include "../commands.defs"

        default:
            printf (
                "ERROR: unknown command at pos %d with value %d, exiting cpu!\n", 
                cpu->ip, cpu->bin[cpu->ip]
            );

            return;
        }
    }

#include "unmeta.h"

}

int *get_arg (const char *bin, Cpu *cpu) {
    assert (bin);

    char type = *bin;
    bin++;
    int data = *(int*) bin;

    switch (type) {
    case ARG_INT:
        // TODO is it safe?
        return (int*) bin;
    
    case ARG_REG:
        return cpu->regs + data;

    // TODO check boundaries of ram
    case ARG_RAM:
        if (data < VID_RAM)
            Sleep (RAM_DELAY);
        else
            Sleep (VID_RAM_DELAY);
        return cpu->ram + data;

    case ARG_REG_RAM:
        if (cpu->regs[data] < VID_RAM)
            Sleep (RAM_DELAY);
        else
            Sleep (VID_RAM_DELAY);
        return cpu->ram + cpu->regs[data];
    
    default:
        // program should not get here in normal situation
        printf ("Unknown type of arg to read, got %d at pos %d, aborting\n", type, cpu->ip);
        abort ();
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

int *safe_pop (Cpu *cpu) {
    assert (cpu);
    
    DEB ("Poping\n");

    if (is_empty_stack (&cpu->stack)) {
        printf ("Poping before initializing stack at pos %d, exiting cpu\n", cpu->ip);
        return nullptr;
    }

    if (!cpu->stack.size) {
        printf ("Poping empty stack at pos %d, exiting cpu\n", cpu->ip);
        return nullptr;
    }

    return (int*) stack_pop (&cpu->stack);
}

void clear_input_buffer () {
    // clears input buffer until the end of line

    while (getchar () != '\n') { ; }
}

bool check_file_data (const char *bin) {
    int asm_sign = 0;
    char asm_ver[8] = "";

    memcpy (&asm_sign, bin, sizeof (asm_sign));
    memcpy (asm_ver, bin + sizeof (asm_sign), sizeof (asm_ver));
    

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


