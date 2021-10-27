#ifndef CPU_CPU_H
#define CPU_CPU_H

#include "../config.h"

#include "../../stack/source/stack.h"

const int NUM_REGS = 16;

const int PIXEL_X = 101;
const int PIXEL_Y = 30;
const int VID_RAM = 2000;
const int NUM_RAM = VID_RAM + PIXEL_X * PIXEL_Y + 10;

const int RAM_DELAY = 0;
const int VID_RAM_DELAY = 0;

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

// TODO should i have this declarations here ?

void execute_cpu (Cpu *cpu);

int *get_arg (const char *bin, Cpu *cpu);

void clear_input_buffer ();

int input_int ();

bool check_file_data (const char *bin);

int *safe_pop (Cpu *cpu);

#endif // CPU_CPU_H
