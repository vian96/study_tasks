#include "commands.h"

#include "../onegin/file_utils.h"
#include "../stack/source/stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

char *create_out_name (const char *in_name);

int *parse_commands (FileText *code, size_t *out_size);

int count_num_args (const char *cmd);

int get_arg (const char **str);

bool is_empty_string (const char *str);

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

    // remove comments
    for (size_t line = 0; line < code.count; line++) {
        char *str = code.strings[line].begin;
        
        while (*str) {
            if (*str == ';') {
                *str = '\0';
                break;
            }
            
            str++;
        }
    }

    size_t out_size = 0;
    int *out = parse_commands (&code, &out_size);

    for (int i = 0; i < out_size; i++)
        printf ("%d ", out[i]);
    
    printf("\n");

    free (out);
    free_file_text (&code);
    free (out_name);

    printf("\nDONE\n");
    return 0;
}

char *create_out_name (const char *in_name) {
    size_t name_len = strlen (in_name);

    char *out_name = (char*) calloc (name_len + 1, sizeof (char)); 
    strcpy (out_name, in_name);
    
    // *name*.boac - binary out assembler code
    out_name[name_len] = '\0';
    out_name[name_len - 4] = 'b';
    out_name[name_len - 3] = 'o';
    out_name[name_len - 2] = 'a';
    out_name[name_len - 1] = 'c';

    return out_name;
}

int *parse_commands (FileText *code, size_t *out_size) {
    assert (code);
    assert (code->strings);
    assert (out_size);
   
    for (size_t line = 0; line < code->count; line++) {
        if (is_empty_string (code->strings[line].begin))
            continue;
                
        AsmCmd cmd = get_cmd (code->strings[line].begin);
        
        if (cmd == -1) {
            printf (
                "Syntax ERROR at line %d: wrong commmand\n%s\n", 
                line + 1, code->strings[line].begin
            );
            return nullptr;
        }

        int args = count_num_args (code->strings[line].begin + strlen (cmd_names[cmd]));

        if (cmd_args[cmd] != args) {
            printf (
                "Syntax ERROR at line %d: expected %d argument after command %s, got %d\n%s\n", 
                line + 1, cmd_args[cmd], cmd_names[cmd], args, code->strings[line].begin
            );
            return nullptr;
        }

        *out_size += 1 + cmd_args[cmd];
    }

    int *out = (int*) calloc (*out_size, sizeof (int));
    size_t f_pos = 0;

    for (size_t line = 0; line < code->count; line++) {
        const char *str = code->strings[line].begin;

        if (is_empty_string (str))
            continue;
        
        AsmCmd cmd = get_cmd (str);

        out[f_pos++] = cmd;

        int cnt = 0;
        
        // skips name of func
        while (*str && !isblank(*str))
            str++;
        
        while (cnt++ < cmd_args[cmd])
            out[f_pos++] = get_arg (&str);
    }

    return out;
}

int count_num_args (const char *cmd) {
    assert (cmd);
    
    int count = 0;

    while (*cmd) {
        // moving to arg
        while (*cmd && isblank(*cmd))
            cmd++;

        if (*cmd)
            count++;

        // skiping arg
        while (*cmd && isalnum (*cmd)) 
            cmd++;
    }

    return count;
}

int get_arg (const char **str) {
    assert (str);
    assert (*str);

    while (**str && isblank(**str))
        (*str)++;

    int res = atoi (*str);

    while (*str && isalnum (**str)) 
        (*str)++;

    return res;
}

bool is_empty_string (const char *str) {
    assert (str);

    while (*str && isblank (*str))
        str++;
    
    return !*str;
}


