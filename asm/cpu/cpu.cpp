#include "../config.h"
#include "../commands.h"
#include "../args.h"
#include "cpu.h"

#include "../../onegin/file_utils.h"
#include "../../stack/source/stack.h"

#include <windows.h>

#include <stdio.h>
#include <math.h>

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

    cpu.ip += FILE_DATA_SIZE;
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

    memset (cpu->regs, 0, NUM_REGS * sizeof (*cpu->regs));
    memset (cpu->ram, 0, NUM_RAM * sizeof (*cpu->ram));

    cpu->stack = {};
    stack_ctor (&cpu->stack, 0, sizeof (int));
}

void cpu_dtor (Cpu *cpu) {
    free ((char*) cpu->bin);

    cpu->bin = nullptr;
    cpu->ip = 0;
    cpu->len = 0;
    
    memset (cpu->regs, 0, sizeof (cpu->regs));
    memset (cpu->ram, 0, sizeof (cpu->ram));
    
    stack_dtor (&cpu->stack);
}

void cpu_dump (FILE *f_out, Cpu *cpu, bool is_ram) {
    assert (f_out);

    if (ferror (f_out)) 
        return;

    fprintf (f_out, "\n-------------CPU DUMP---------------\n");

    if (!cpu) {
        fprintf (
            f_out,
            "ERROR: CPU address is NULL!!!!!\n"
            "--------------------------------------\n"
        );

        return;
    }

    fprintf (f_out, "Address of cpu: %p\n", cpu);

    fprintf (
        f_out, 
        "Address of binary code: %p\n"
        "Len of binary code: %d\n"
        "Current instruction pointer: %d\n"
        "Address of regs: %p\n"
        "Address of ram: %p\n",
        cpu->bin, cpu->len, cpu->ip, cpu->regs, cpu->ram
    );

    stack_dump (&cpu->stack, f_out, MAX_DUMP);

    if (is_ram)
        ram_dump (f_out, cpu);
    regs_dump (f_out, cpu);

    fprintf (
        f_out,
        "\n\n---------END OF CPU DUMP------------\n\n"
    );
}

void ram_dump (FILE *f_out, Cpu *cpu) {
    assert (f_out);

    if (ferror (f_out)) 
        return;

    fprintf (f_out, "\n------RAM dump------\n");
    
    const int LEN_SIZE = 25;

    fprintf (f_out, "    ");
    for (int x = 0; x < LEN_SIZE; x++)
        fprintf (f_out, "%4d", x);
    fputc ('\n', f_out);

    for (int y = 0; y < (NUM_RAM - 1) / LEN_SIZE + 1; y++) {
        fprintf (f_out, "%4d", y);
        
        for (int x = 0; x < LEN_SIZE && x + y * LEN_SIZE < NUM_RAM; x++)
            fprintf (f_out, "%4d", cpu->ram[y * LEN_SIZE + x]);

        fputc ('\n', f_out);
    }
    
    fprintf (f_out, "\n------END of RAM dump------\n\n");
}

void regs_dump (FILE *f_out, Cpu *cpu) {
    assert (f_out);

    if (ferror (f_out)) 
        return;

    fprintf (f_out, "\n------REGS dump------\n");
    
    for (int x = 0; x < NUM_REGS; x++)
        fprintf (f_out, "%4cx", 'a' + x);
    fputc ('\n', f_out);

    for (int x = 0; x < NUM_REGS; x++)
        fprintf (f_out, "%5d", cpu->regs[x]);
    fputc ('\n', f_out);
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
        if (data < 0 || data > NUM_REGS) {
            printf ("Wrong reg number, trying to access %d, regs count is %d, exiting cpu\n", data, NUM_RAM);
            return nullptr;
        }

        return cpu->regs + data;

    case ARG_RAM:
        if (data < 0 || data >= NUM_RAM) {
            printf ("Out of boundaries of ram, trying to access %d, ram size is %d, exiting cpu\n", data, NUM_RAM);
            return nullptr;
        }

        if (data < VID_RAM)
            Sleep (RAM_DELAY);
        else
            Sleep (VID_RAM_DELAY);
        return cpu->ram + data;

    case ARG_REG_RAM:
        if (data < 0 || data > NUM_REGS) {
            printf ("Wrong reg number, trying to access %d, regs count is %d, exiting cpu\n", data, NUM_RAM);
            return nullptr;
        }

        if (cpu->regs[data] < 0 || cpu->regs[data] >= NUM_RAM) {
            printf ("Out of boundaries of ram, trying to access %d, ram size is %d, exiting cpu\n", data, NUM_RAM);
            return nullptr;
        }

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
        printf (
            "Poping before initializing stack at pos %d with cmd: %s, exiting cpu\n", 
            cpu->ip, cmd_names[cpu->bin[cpu->ip - 1]]
        );
        return nullptr;
    }

    if (!cpu->stack.size) {
        printf (
            "Poping empty stack at pos %d with cmd: %s, exiting cpu\n", 
            cpu->ip, cmd_names[cpu->bin[cpu->ip - 1]]
        );
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


