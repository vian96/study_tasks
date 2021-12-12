#include "diff_tree.h"

#include <stdio.h>

int main ()
    {
    /*
    const char *expr = "(((x)^(0.6))^(2))", // "((((1)+(((3)+((2)+(x)))+(6)))^(0.6))^(2))", 
                **to_read = &expr;

    DiffTree *tree = read_expression (to_read, nullptr);
    dt_to_latex (tree);
    printf ("\n\n");
    simplify_diff_tree (tree);
    //printf ("\nTST\n");
    //dt_calc_close (tree);
    //diff_tree_dump (tree);
    printf ("\n\n");
    dt_to_latex (tree);
    printf ("\n\n");
    print_tree (tree);
    printf ("\n");
    diff_tree_dtor (tree);

    //*/

    
    const char *expr = "((((((x)^(2))+(2))^(0.5))-((((0.5)*((((x)^(2))+(2))^(-0.5)))*((x)*(2)))*((x)+(2))))/(((x)^(2))+(2)))", //"(((x)+(2))/((((x)^(2))+(2))^(0.5)", // "((((1)*((x)+(0)))/(1))^(0))", // "(((27)+((x)*(38)))^((34)/((27)/(4)))",
        **to_read = &expr;

    DiffTree *tree = read_expression (to_read, nullptr);
    //print_tree (tree);
    printf ("\n");
    // dt_to_latex (tree);
    printf ("\n\n");

    check_diff_tree (tree);

    printf ("\n\nStarting differentiating\n");
    DiffTree *diff = dt_differ (tree);
    printf ("\n\n");
    dt_to_latex (diff);
    printf ("\n\n");
    print_tree (diff);
    printf ("\n\n");

    if (check_diff_tree (diff))
        printf ("DIFF IS FULLY OK\n");
    else   
        printf ("DIFF IS BROKEN AS HELL!!!!!!!!!\n");

    printf ("\nSimplifying\n\n");
    simplify_diff_tree (diff);
    printf ("\n\n");

    if (check_diff_tree (diff))
        printf ("simplified IS FULLY OK\n");
    else   
        printf ("simplified IS BROKEN AS HELL!!!!!!!!!\n");

    // diff_tree_dump (diff);

    dt_to_latex (diff);
    printf ("\n\n");
    print_tree (diff);
    printf ("\n\n");

    printf ("Start freeing\n");
    diff_tree_dtor (diff);
    diff_tree_dtor (tree);
    //*/
    
    printf ("DONE\n");
    }

