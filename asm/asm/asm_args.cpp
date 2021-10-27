#include "asm_args.h"
#include "string_utils.h"
#include "labels.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

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

void set_int (char *dest, int val) {
    assert (dest);

    memcpy (dest, &val, sizeof (int));
}

void set_arg (const char **str, char *out) {
    assert (str);
    assert (*str);
    assert (out);

    skip_blank (str);

    // TODO maybe use some mactos to reduce code?
    switch (get_type_arg (*str)) {
    case ARG_INT: {
        out[0] = ARG_INT;
        out++;

        int res = 0;
        sscanf (*str, "%d", &res);
        set_int (out, res);
        
        break;
    }
    case ARG_REG:
        out[0] = ARG_REG;
        out++;

        set_int (out, (*str)[0] - 'A');

        break;
    
    case ARG_RAM: {
        out[0] = ARG_RAM;
        out++;

        int res = 0;
        sscanf (*str + 1, "%d", &res);
        set_int (out, res);

        break;
    }
    case ARG_REG_RAM:
        out[0] = ARG_REG_RAM;
        out++;

        set_int (out, (*str)[1] - 'A');

        break;

    case ARG_LABEL: {
        out[0] = ARG_INT;
        out++;

        int res = get_label_val (*str + 1);
        if (res == -1) {
            printf ("Error at line trying to get label value:\n%s\n", str);
            return;
        }

        set_int (out, res);

        break;
    }
    default:
        printf ("Unknown type of arg at line:\n%s\n", str);
        return;
    }

    skip_alnum (str);
}

int get_type_arg (const char *str) {
    assert (str);

    int res = 0;

    if (str[0] == ':') {
        return ARG_LABEL;
    }
    
    if (isalpha (str[0]) && str[1] == 'X' && (isspace (str[2]) || !str[2] || str[2] == ']'))
        return ARG_REG;

    if (sscanf (str, "%d", &res)) 
        return ARG_INT;
    
    if (str[0] == '[') { 
        // RAM, need to identify how you access it
        char *bracket = (char*) strchr (str, ']');
        
        if (!bracket) {
            printf ("Syntax ERROR: unclosed '[' at line:\n%s\n", str);
            return 0;
        }
        
        int arg = get_type_arg (str + 1);

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
    
    // unknown type of argument
    return 0;
}

