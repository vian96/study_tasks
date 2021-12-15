#include "diff_tree.h"

#include "tex_text.h"

#include <stdio.h>

int main ()
    {
    // Feel free to use any of these expressions (if you are breave enough of course!)
    // 
    //"(ln((cos(x))+(sin(cos((x)^(2))))))", 
    // "((((((x)^(2))+(2))^(0.5))-((((0.5)*((((x)^(2))+(2))^(-0.5)))*((x)*(2)))*((x)+(2))))/(((x)^(2))+(2)))", 
    //"(((x)+(2))/((((x)^(2))+(2))^(0.5)", 
    // "((((1)*((x)+(0)))/(1))^(0))", 
    // "(((27)+((x)*(38)))^((34)/((27)/(4)))",

    const char *expr = "(ln((cos(x))+(sin(cos((x)^(2))))))", 
        **to_read = &expr;

    DiffTree *tree = read_expression (to_read, nullptr);
    
    FILE *f_out = fopen ("diff_out.tex", "w");
    fprintf (f_out, tex_text[0]);

    dt_to_latex (tree, f_out);
    fprintf (f_out, tex_text[1]);

    DiffTree *diff = dt_differ (tree);
    dt_to_latex (diff, f_out);
    fprintf (f_out, tex_text[2]);
    simplify_diff_tree (diff);
    dt_to_latex (diff, f_out);
    fprintf (f_out, tex_text[3]);
    
    diff_tree_dtor (diff);
    diff_tree_dtor (tree);
    //*/
    
    printf ("DONE\n");
    }

