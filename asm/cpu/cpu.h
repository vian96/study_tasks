#ifndef CPU_CPU_H
#define CPU_CPU_H

#include "../config.h"

#include "../../stack/source/stack.h"

const int NUM_REGS = 16;

const int PIXEL_X = 100;
const int PIXEL_Y = 30;
const int VID_RAM = 5000; // begin of video ram
const int STATIC_RAM = 5000; // number of static ram
const int NUM_RAM = VID_RAM + PIXEL_X * PIXEL_Y + 10 + STATIC_RAM;

const int RAM_DELAY = 0;
const int VID_RAM_DELAY = 0;
const int PRINT_DELAY = 60;

struct Cpu {
    const char *bin;
    int len;
    int ip;

    Stack stack;
    int regs [NUM_REGS];
    int ram  [NUM_RAM];
};

void cpu_ctor (Cpu *cpu);

void cpu_dtor (Cpu *cpu);

void cpu_dump (FILE *f_out, Cpu *cpu, bool is_ram = 0);

void ram_dump (FILE *f_out, Cpu *cpu);

void regs_dump (FILE *f_out, Cpu *cpu);

// TODO should i have this declarations here ?

void execute_cpu (Cpu *cpu);

int *get_arg (const char *bin, Cpu *cpu);

void clear_input_buffer ();

int input_int ();

bool check_file_data (const char *bin);

int *safe_pop (Cpu *cpu);

#endif // CPU_CPU_H
