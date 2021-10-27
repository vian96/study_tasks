#include "asm.h"

#include "../config.h"

#include "../commands.h"
#include "string_utils.h"
#include "labels.h"
#include "asm_args.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

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

    prepare_file_text (&code);

    size_t out_size = 0;
    char *out = parse_commands (&code, &out_size);

    free_file_text (&code);

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

// TODO is it okay to have implementations here ?

char *create_out_name (const char *in_name) {
    size_t name_len = strlen (in_name);
    char *out_name = strdup (in_name);
    
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
   
    DEB ("Calculating out size...\n");
    *out_size = get_out_size (code);

    if (!*out_size)
        return nullptr;

    char *out = (char*) calloc (*out_size, sizeof (char));
    
    DEB ("Starting the first run...\n");
    size_t f_pos = create_out_arr (code, out);
    assert (f_pos == *out_size);

    // TODO remove second run if it is not needed (e.g. no exmpty labels)
    
    // second run for jumps
    DEB ("Starting the second run..\n");
    f_pos = create_out_arr (code, out);
    assert (f_pos == *out_size);

    return out;
}

AsmCmd get_cmd (const char *str) {
    for (int i = 0; i < NUM_CMD; i++)
        if (is_equal_words (str, cmd_names[i]) == 1)
            return (AsmCmd) i;

    return (AsmCmd) -1;
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
        
        if (str[0] == ':') {
            // label
            str++; // skips ':'

            skip_name (&str);
            skip_blank (&str);
            
            if (*str) {
                printf (
                    "Syntax ERROR at line %d: symbols after name of label\n%s\n", 
                    line + 1, str
                );
            }

            continue;
        }

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

        out_size += SIZE_CMD + SIZE_ARG * cmd_args[cmd];
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

        if (str[0] == ':') {
            // label
            if (set_label_val (str + 1, f_pos + FILE_DATA_SIZE) == -1) {
                printf ("Error at line trying to set label value:\n%s\n", str);
                return 0;
            }

            continue;
        }
        
        AsmCmd cmd = get_cmd (str);

        out[f_pos] = cmd;
        f_pos += SIZE_CMD;
        
        // skips name of func
        skip_alnum (&str);

        int cnt = 0;
        
        while (cnt++ < cmd_args[cmd]) { 
            set_arg (&str, out + f_pos);
            f_pos += SIZE_ARG;
        }
    }

    return f_pos;
}


