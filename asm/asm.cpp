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

char get_type_arg (const char *str);

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

    if (!*out_size)
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
        const char *str = code->strings[line].begin;
        
        if (is_empty_string (str))
            continue;
                
        skip_blank (&str);
        
        AsmCmd cmd = get_cmd (str);
        
        if (cmd == -1) {
            printf (
                "Syntax ERROR at line %d: wrong commmand\n%s\n", 
                line + 1, str
            );
            return 0;
        }

        int args = count_num_args (str + strlen (cmd_names[cmd]));

        if (args == -1) {
            printf (
                "Syntax ERROR at line %d: unrecognised argument after command %s\n%s\n", 
                line + 1, cmd_names[cmd], str
            );

            return 0;
        }

        if (cmd_args[cmd] != args) {
            printf (
                "Syntax ERROR at line %d: expected %d argument after command %s, got %d\n%s\n", 
                line + 1, cmd_args[cmd], cmd_names[cmd], args, str
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
        
        skip_blank (&str);
        
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

        if (*cmd) {
            if (!get_type_arg (cmd))
                return -1;

            count++;
        }
        
        skip_not_blank (&cmd); // skiping arg
    }

    return count;
}

void set_arg (const char **str, char *out) {
    assert (str);
    assert (*str);
    assert (out);

    skip_blank (str);

    // TODO functions get_reg, get_int, set_int and so on
    switch (get_type_arg (*str)) {
    case ARG_INT: {
        int res = 0;
        const char *bytes = (const char*) &res;

        sscanf (*str, "%d", &res);
        
        out[0] = ARG_INT;
        out++;

        for (int i = 0; i < sizeof (int); i++)
            out[i] = bytes[i];

        break;
    }
    case ARG_REG:
        out[0] = ARG_REG;
        out++;

        out[0] = (*str)[0] - 'A';
        for (int i = 1; i < sizeof (int); i++)
            out[i] = 0;

        break;
    
    case ARG_RAM: {
        out[0] = ARG_RAM;
        out++;

        int res = 0;
        const char *bytes = (const char*) &res;

        sscanf (*str + 1, "%d", &res);

        for (int i = 0; i < sizeof (int); i++)
            out[i] = bytes[i];

        break;
    }
    case ARG_REG_RAM:
        out[0] = ARG_REG_RAM;
        out++;

        out[0] = (*str)[1] - 'A';
        for (int i = 1; i < sizeof (int); i++)
            out[i] = 0;

        break;

    default:
        // TODO change assert to verify ...
        // program should not get here
        assert (("Unknown type of arg", 0));
        break;
    }

    skip_alnum (str);
}

char get_type_arg (const char *str) {
    assert (str);

    int res = 0;

    if (sscanf (str, "%d", &res)) 
        return ARG_INT;
    
    if (str[1] == 'X' && (isspace (str[2]) || !str[2]))
        return ARG_REG;

    if (str[0] == '[') { 
        // RAM, need to identify how you access
        char *bracket = (char*) strchr (str, ']');
        
        if (!bracket) {
            printf ("Syntax ERROR: unclosed '[' at line:\n%s\n", str);
            return 0;
        }
        
        *bracket = '\0';
        char arg = get_type_arg (str + 1);
        *bracket = ']';

        switch (arg) {
        case ARG_INT:
            return ARG_RAM;
            
        case ARG_REG:
            return ARG_REG_RAM;

        default:
            printf ("Unknown type of address of ram at line:\n%s\n", str);
            return 0;
        }
    }

    if (str[0] == ':') {
        // label
        return ARG_LABEL;
    }
    
    // unknown type of argument
    return 0;
}




