#include "commands.h"

#include "../onegin/file_utils.h"
#include "../stack/source/stack.h"

#define NDEBUG

#ifndef NDEBUG
#define deb(...) printf (__VA_ARGS__);
#else // debug
#define deb(...) ;
#endif // not debug


const int CMD_SIZE = sizeof (int);
const int ARG_SIZE = sizeof (int);

int get_arg (const char *bin);

void clear_input_buffer ();

int input_int ();

uint64_t reverse_bytes (uint64_t bytes);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf (
            "ERROR: wrong number of arguments\n"
            "You shold call \"cpu *name*.boac\"!\n"
        );
        return 0;
    }

    if (strlen (argv[1]) < 6 || strcmp (argv[1] + strlen (argv[1]) - 5, ".boac") != 0) {
        printf (
            "ERROR: wrong name of input file\n"
            "Input file should have name \"*name*.boac\" where len(name) > 0\n"
        );
    } 

    const char *in_name = argv[1];

    FILE *f_in = fopen_err (in_name, "rb");
    int file_len = 0;

    char *bin = read_symbols_file (f_in, &file_len);

    fclose (f_in);

    int ip = 0;

    // TODO remove %4 when change to variable length
    if (sizeof (ASM_SIGN) + sizeof (ASM_VER) >= file_len || file_len % 4 != 0) {
        printf (
            "ERROR: file is corrupted, it is too small or has bad length\n"
        );
        free (bin);
        
        return 0;
    }

    //FileData file_data = {};
    int asm_sign = 0;
    char asm_ver[8] = "";

    memcpy (&asm_sign, bin, sizeof (asm_sign));
    ip += sizeof (asm_sign);

    memcpy (&asm_ver, bin + ip, sizeof (asm_ver));
    ip += sizeof (asm_ver);

    if (asm_sign != ASM_SIGN) {
        printf ("ERROR: file is corrupted, signature is not okay\n");
        free (bin);
        
        return 0;
    }

    if (strcmp (asm_ver, ASM_VER) != 0) {
        printf ("File version is not compatible with this cpu, expected: %s, got: %s\n", ASM_VER, asm_ver);
        free (bin);
        
        return 0;
    }

    deb ("Starting executing..\n");

    Stack stack = {};
    stack_ctor (&stack, 0, sizeof (int));

    deb ("Initialized stack..\n");
    
#define PUSH(val) {int VAL__ = (val); stack_push (&stack, &VAL__);}
#define POP (*(int*) stack_pop (&stack))
#define ARG (get_arg (bin + ip))

    while (ip < file_len) {
        deb ("while..\n");

        switch (*(AsmCmd*) (bin + ip)) {
        case CMD_PUSH:
            deb ("push..\n");
            ip += CMD_SIZE;
            deb ("CMD: push ip: %d, arg: %d\n", ip, ARG);
            
            PUSH(ARG);
            deb ("pushed..\n");

            ip += ARG_SIZE;

            break;
        case CMD_IN:
            ip += CMD_SIZE;
            
            PUSH (input_int ());
            deb ("Pushed in..\n");

            break;
        case CMD_MUL:
            ip += CMD_SIZE;
            
            deb ("Mul..\n");
            PUSH (POP * POP);
            deb ("Pushed mul..\n");
            
            break;
        case CMD_OUT:
            ip += CMD_SIZE;

            printf ("OUT: %d\n", POP);
            
            break;
        case CMD_ADD:
            ip += CMD_SIZE;

            deb ("Adding numbers..\n");
            PUSH (POP + POP);
            deb ("Pushed add...\n");

            break;
        default:
            deb ("ERROR: unknown command, exiting cpu!\n");
            free (bin);
            return 0;
        }
    }

    printf ("DONE\n");

    free (bin);
}

int get_arg (const char *bin) {
    assert (bin);

    return *(int*) bin;
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

