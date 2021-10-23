#include "commands.h"
#include "string_utils.h"

#include "../onegin/file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

// TODO change int to int32_t or somthing

char *create_out_name (const char *in_name);

char *parse_commands (const FileText *code, size_t *out_size);

size_t get_out_size (const FileText *code);

size_t create_out_arr (const FileText *code, char *out);

int count_num_args (const char *cmd);

void set_arg (const char **str, char *out);

int main (int argc, char *argv[]) {
    if (argc != 2) {
        printf (
            "ERROR: wrong number of arguments\n"
            "You shold call \"asm *name*.casm\"!\n"
        );
        return 0;
    }

    if (strlen (argv[1]) < 6 || strcmp (argv[1] + strlen (argv[1]) - 5, ".casm") != 0) {
        printf (
            "ERROR: wrong name of input file\n"
            "Input file should have name \"*name*.casm\" where len(name) > 0\n"
        );
    }

    const char *in_name = argv[1];
    size_t name_len = strlen (in_name);

    char *out_name = create_out_name (in_name);

    printf ("Code will be compiled into file %s\n", out_name);

    FILE *f_in = fopen_err (in_name, "r");
    if (!f_in)
        return 0;
    
    FileText code = read_text_file (f_in);

    fclose (f_in);

    for (size_t line = 0; line < code.count; line++) {
        char *str = code.strings[line].begin;
        
        while (*str) {
            // converts string to upper case
            *str = toupper (*str);

            // removes comments
            if (*str == ';') {
                *str = '\0';
                break;
            }
            
            str++;
        }
    }

    size_t out_size = 0;
    char *out = parse_commands (&code, &out_size);

    free_file_text (&code);

    for (int i = 0; i < out_size; i++)
        printf ("%d ", out[i]);
    
    printf ("\n");

    FILE *f_out = fopen_err (out_name, "wb");
    if (!f_out)
        return 0;

    fwrite (&ASM_SIGN, sizeof (ASM_SIGN), 1, f_out);
    fwrite (&ASM_VER, sizeof (ASM_VER), 1, f_out);

    fwrite (out, sizeof (*out), out_size, f_out);

    fclose (f_out);

    free (out);
    free (out_name);

    printf("\nDONE\n");
    return 0;
}

char *create_out_name (const char *in_name) {
    size_t name_len = strlen (in_name);

    char *out_name = (char*) calloc (name_len + 1, sizeof (char)); 
    strcpy (out_name, in_name);
    
    // *name*.boac - binary out assembler code
    out_name[name_len - 4] = 'b';
    out_name[name_len - 3] = 'o';
    out_name[name_len - 2] = 'a';
    out_name[name_len - 1] = 'c';
    out_name[name_len    ] = '\0';

    return out_name;
}

char *parse_commands (const FileText *code, size_t *out_size) {
    assert (code);
    assert (code->strings);
    assert (out_size);
   
    *out_size = get_out_size (code);

    if (!out_size)
        return nullptr;

    char *out = (char*) calloc (*out_size, 1);
    
    size_t f_pos = create_out_arr (code, out);
    
    assert (f_pos == *out_size);

    // second run for jumps

    return out;
}

size_t get_out_size (const FileText *code) {
    assert (code);
    assert (code->strings);

    size_t out_size = 0;

    for (size_t line = 0; line < code->count; line++) {
        if (is_empty_string (code->strings[line].begin))
            continue;
                
        AsmCmd cmd = get_cmd (code->strings[line].begin);
        
        if (cmd == -1) {
            printf (
                "Syntax ERROR at line %d: wrong commmand\n%s\n", 
                line + 1, code->strings[line].begin
            );
            return 0;
        }

        int args = count_num_args (code->strings[line].begin + strlen (cmd_names[cmd]));

        if (cmd_args[cmd] != args) {
            printf (
                "Syntax ERROR at line %d: expected %d argument after command %s, got %d\n%s\n", 
                line + 1, cmd_args[cmd], cmd_names[cmd], args, code->strings[line].begin
            );
            return 0;
        }

        out_size += CMD_SIZE + ARG_SIZE * cmd_args[cmd];
    }

    return out_size;
}

size_t create_out_arr (const FileText *code, char *out) {
    assert (code);
    assert (code->strings);
    assert (out);

    size_t f_pos = 0;

    for (size_t line = 0; line < code->count; line++) {
        const char *str = code->strings[line].begin;

        if (is_empty_string (str))
            continue;
        
        AsmCmd cmd = get_cmd (str);

        out[f_pos] = cmd;
        f_pos += CMD_SIZE;
        
        // skips name of func
        skip_not_blank (&str);

        int cnt = 0;
        
        while (cnt++ < cmd_args[cmd]) { 
            set_arg (&str, out + f_pos);
            f_pos += ARG_SIZE;
        }
    }

    return f_pos;
}

int count_num_args (const char *cmd) {
    assert (cmd);
    
    int count = 0;

    while (*cmd) {
        skip_blank (&cmd); // moving to arg

        if (*cmd)
            count++;
        
        skip_not_blank (&cmd); // skiping arg
    }

    return count;
}

void set_arg (const char **str, char *out) {
    assert (str);
    assert (*str);
    assert (out);

    skip_blank (str);

    int res = atoi (*str);
    char *bytes = (char*) &res;

    for (int i = 0; i < sizeof (int); i++)
        out[i] = bytes[i];

    skip_alnum (str);
}


