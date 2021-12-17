#include "diff_tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

//#define NDEBUG

#ifndef NDEBUG
#define DEB(...) printf (__VA_ARGS__)
#else
#define DEB(...) 
#endif

void diff_tree_ctor (DiffTree *tree, DiffTreeType type, DiffTree *parent, DiffTreeData data, DiffTree *left, DiffTree *right)
    {
    assert (tree);

    tree->type = type;
    tree->parent = parent;
    tree->data = data;

    tree->left = left;
    tree->right = right;
    }

DiffTree *new_diff_tree_ctor (DiffTreeType type, DiffTree *parent, DiffTreeData data, DiffTree *left, DiffTree *right)
    {
    DiffTree *tree = (DiffTree*) calloc (1, sizeof (*tree));
    diff_tree_ctor (tree, type, parent, data, left, right);
    return tree;
    }

void diff_tree_dtor (DiffTree *tree)
    {
    assert (tree);

    if (tree->type == DT_NUMBER || tree->type == DT_OPERATOR)
        tree->data.number = 0; // because number is the biggest part of data
    else if (tree->type == DT_VAR)
        {
        free (tree->data.var);
        tree->data.var = nullptr;
        }        

    tree->type = (DiffTreeType) 0;
    tree->parent = 0;

    if (tree->left)
        diff_tree_dtor (tree->left);
    tree->left = nullptr;
    
    if (tree->right)
        diff_tree_dtor (tree->right);
    tree->right = nullptr;

    free (tree);
    }

// TODO divide into smaller functions like read_id, read_num ...
DiffTree *read_expression (const char **str, DiffTree *parent)
    {
    assert (str);
    assert (*str);

    DEB ("Got line to read: %s, start analyzing\n", *str);
    if (**str != '(')
        {
        printf ("ERROR: you passed to read_expression string without '(' in begin, it is: %s\n", 
                    *str);
        return nullptr;
        }
    (*str)++;

    // const pointer is needed because of to_ret
    DiffTree * const tree = new_diff_tree_ctor ((DiffTreeType) 0, parent); 
    DiffTree * to_ret = tree;
    if (parent == nullptr)
        {
        parent = new_diff_tree_ctor (DT_EXPRESSION);
        parent->left = tree;
        tree->parent = parent;
        to_ret = parent;
        }

    double num = NAN;
    if (sscanf (*str, "%lf", &num))
        {
        // found a number
        DEB ("Found a number\n");
 
        *str = strchr (*str, ')');
        (*str)++; // because it will point to ')' after search

        tree->type = DT_NUMBER;
        tree->data.number = num;

        return to_ret;
        }

    DEB ("The current symbol is %d %c\n", (unsigned char)**str, **str);
    if (**str == '(')
        {
        DEB ("Found binary operator\n");
        
        tree->type = DT_OPERATOR;
        tree->left = read_expression (str, tree);
        tree->data.oper = read_dt_oper (str);
        tree->right = read_expression (str, tree);
        
        (*str)++;   // because it will point to ')' after search
        return to_ret;
        }

    DiffTreeOper oper = read_dt_oper (str);
    if (oper)
        {
        DEB ("Found unary operator\n");
        tree->type = DT_OPERATOR;
        tree->data.oper = oper;
        tree->right = read_expression (str, tree);;

        (*str)++;   // because it will point to ')' after search
        return to_ret;
        }

    // identifier 
    DEB ("Found identifier, current position is %s\n", *str);

    tree->type = DT_VAR;
    const char *end = strchr (*str, ')');
    DEB ("Str was at %c and i moved it to %c\n", **str, *end);
    // evil pointer arithmetic is to calculate len of identifier, +1 is for '\0'
    char *name = (char*) calloc (end - *str + 1, sizeof (*name));
    strncpy (name, *str, end - *str);
    if (tree->data.var)
        printf ("Something is wrong, var is not empty, it is %p\n", tree->data.var);
        
    tree->data.var = name;
    *str = end + 1; // +1 because end points to ')'
    DEB ("This identifier is %s\n", tree->data.var);

    return to_ret;
    }

DiffTreeOper read_dt_oper (const char **str)
    {
    assert (str);
    assert (*str);
    
    DEB ("Reading operator at %s\n", *str);
    switch (**str)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '^':
        {
        DiffTreeOper oper = (DiffTreeOper) (**str);
        (*str)++;
        return oper;
        }
        
    // Why should i care if user wrote it okay?
    case 'l':
        (*str) += 2;
        return DTO_LN;
    case 's':
        (*str) += 3;
        return DTO_SIN;
    case 'c':
        (*str) += 3;
        return DTO_COS;

    default:
        break;
    }
    
    DEB ("Haven't found any operator, leaving str where it was\n");
    return DTO_INVALID;
    }

void print_tree (const DiffTree *tree)
    {
    assert (tree);

    if (tree->type == DT_EXPRESSION)
        {
        print_tree (tree->left);
        return;
        }

    switch (tree->type)
    {
    case DT_VAR:
        printf ("(%s)", tree->data.var);
        return;
    case DT_NUMBER:
        printf ("(%g)", tree->data.number);
        return;
    
    case DT_OPERATOR:
        printf ("(");
        if (tree->left)
            print_tree (tree->left);
        
        print_oper (tree->data.oper);
        if (tree->right)
            print_tree (tree->right);

        printf (")");
        return;

    default:
        printf ("\nERROR: unknown type of node while printing, got: %d\n", tree->type);
         if (tree->left)
            print_tree (tree->left);
        else
            printf ("null");

        printf (" was L and R is ");
        if (tree->right)
            print_tree (tree->right);
        else
            printf ("null");
        
        break;
    }
    }

void print_oper (DiffTreeOper oper)
    {
    switch (oper)
    {
    case DTO_MINUS:
    case DTO_PLUS:
    case DTO_MUL:
    case DTO_DIV:
    case DTO_POW:
        printf ("%c", oper);
        return;
    
    case DTO_LN:
        printf ("ln");
        return;

    case DTO_SIN:
        printf ("sin");
        return;

    case DTO_COS:
        printf ("cos");
        return;
    
    default:
        printf ("Error unknown oper while printing tree\n");
        break;
    }
    }

bool is_bracket_needed (DiffTree *tree)
    {
    assert (tree);

    return oper_precedence (tree->parent->data.oper) < oper_precedence (tree->data.oper) &&        
            tree->parent->data.oper != DTO_DIV                                          
            ||                                              
            oper_precedence (tree->parent->data.oper) == oper_precedence (tree->data.oper) &&       
            (tree->parent->data.oper == DTO_MINUS || tree->parent->data.oper == DTO_POW);
    }

#define is_op(node, op) ( (node)->type == DT_OPERATOR && (node)->data.oper == (op) )

#define op_brack { if (is_bracket_needed (tree)) fprintf (f_out, " {\\left( "); }
#define cl_brack { if (is_bracket_needed (tree)) fprintf (f_out, " \\right)} "); }

void dt_to_latex (DiffTree *tree, FILE *f_out)
    {
    assert (tree);

    if (tree->type == DT_EXPRESSION)
        {
        dt_to_latex (tree->left, f_out);
        return;
        }

    switch (tree->type)
    {
    case DT_VAR:
        fprintf (f_out, "{%s}", tree->data.var);
        return;
    
    case DT_NUMBER:
        if (tree->data.number >= 0)
            fprintf (f_out, "{%g}", tree->data.number);
        else
            fprintf (f_out, "{\\left(%g\\right)}", tree->data.number);
        return;

    case DT_OPERATOR:
        switch (tree->data.oper)
        {
        case DTO_MUL:
            op_brack;
            dt_to_latex (tree->left, f_out);

            fprintf (f_out, " \\cdot ");

            dt_to_latex (tree->right, f_out);
            cl_brack;
            return;
        
        case DTO_PLUS:
            op_brack;
            dt_to_latex (tree->left, f_out);

            fprintf (f_out, " + ");
            
            dt_to_latex (tree->right, f_out);
            cl_brack;
            return;

        case DTO_MINUS:
            op_brack;
            dt_to_latex (tree->left, f_out);

            fprintf (f_out, " - ");
            
            dt_to_latex (tree->right, f_out);
            cl_brack;
            return;

        case DTO_DIV:
            fprintf (f_out, " \\frac{ ");
            dt_to_latex (tree->left, f_out);
            
            fprintf (f_out, " }{ ");
            
            dt_to_latex (tree->right, f_out);
            fprintf (f_out, " } ");
            return;

        case DTO_POW:
            op_brack;
            dt_to_latex (tree->left, f_out);
            
            fprintf (f_out, " ^ ");
            
            fprintf (f_out, "{");
            dt_to_latex (tree->right, f_out);
            fprintf (f_out, "}");
            cl_brack;
            return;

        case DTO_LN:
            fprintf (f_out, " \\ln\\left(");
            dt_to_latex (tree->right, f_out);
            fprintf (f_out, " \\right)");
            return;

        case DTO_SIN:
            fprintf (f_out, " \\sin\\left(");
            dt_to_latex (tree->right, f_out);
            fprintf (f_out, " \\right)");
            return;

        case DTO_COS:
            fprintf (f_out, " \\cos\\left(");
            dt_to_latex (tree->right, f_out);
            fprintf (f_out, " \\right)");
            return;

        default:
            printf ("ERROR: unknown type of operator while latex, i got: %c\n", tree->data.oper);
            return;
        }

    default:
        printf ("\nERROR: unknown type of node while latex, got: %d\n", tree->type);
        break;
    }
    }

#undef is_op
#undef op_brack
#undef cl_brack

DiffTree *new_dt_op (DiffTreeOper oper, DiffTree *left, DiffTree *right, DiffTree *parent)
    {
    DiffTree *to_ret = new_diff_tree_ctor (DT_OPERATOR, parent, new_oper_data (oper), left, right);
    if (left)
        to_ret->left->parent = to_ret;
    if (right)
        to_ret->right->parent = to_ret;
    return to_ret;
    }

// TODO move defines to another file
#define new_op(oper, left, right) ( new_dt_op (oper, left, right, parent) )
#define new_const(value) ( new_diff_tree_ctor (DT_NUMBER, parent, {value}) )

#define L ((tree)->left)
#define R ((tree)->right)
#define D(node) (dt_differ (node, parent))
#define C(node) (copy_diff_tree (node, nullptr))

DiffTree *dt_differ (DiffTree *tree, DiffTree *parent)
    {
    assert (tree);

    if (tree->type == DT_EXPRESSION)
        {
        DiffTree *to_ret = new_diff_tree_ctor (DT_EXPRESSION);
        to_ret->left = dt_differ (tree->left, to_ret);
        return to_ret;
        }

    // null parent is only allowed when tree is DT_expression
    assert (parent);

    switch (tree->type)
    {
    case DT_NUMBER: 
        return new_const (0);

    case DT_VAR:
        // TODO create differ for different vars
        return new_const (1);
    
    case DT_OPERATOR:
        switch (tree->data.oper)
        {
        case DTO_PLUS:
            DEB ("Differentiating plus\n");
            return new_op (DTO_PLUS, D (L), D (R));
        case DTO_MINUS:
            DEB ("Differentiating minus\n");
            return new_op (DTO_MINUS, D (L), D (R));
        case DTO_MUL:
            DEB ("Differentiating mul\n");
            return new_op (DTO_PLUS, new_op (DTO_MUL, D(L), C(R)), new_op (DTO_MUL, D(R), C(L)));
        case DTO_DIV:
            DEB ("Differentiating div\n");
            return new_op (DTO_DIV, 
                        new_op (DTO_MINUS, 
                            new_op (DTO_MUL, D(L), C(R)), new_op (DTO_MUL, D(R), C(L))),
                        new_op (DTO_POW, C (R), new_const (2)));
        // TODO expand it to non-constant power
        case DTO_POW:
            DEB ("Differentiating pow\n");
            return new_op (DTO_MUL, new_op (DTO_MUL, 
                            C (R), new_op (DTO_POW, 
                                C (L), new_op (DTO_MINUS, C (R), new_const (1)))), D (L));
        case DTO_LN:
            DEB ("Differentiating ln\n");
            return new_op (DTO_DIV, D (R), C (R));
        case DTO_SIN:
            DEB ("Differentiating sin\n");
            return new_op (DTO_MUL, new_op (DTO_COS, nullptr, C (R)), D (R));
        case DTO_COS:
            DEB ("Differentiating cos\n");
            return new_op (DTO_MUL, new_const (-1), 
                        new_op (DTO_MUL, new_op (DTO_SIN, nullptr, C (R)), D (R)));

        default:
            printf ("ERROR: unnknown type of operator while differ, it is %c\n", tree->data.oper);
            return nullptr;
        }
    default:
        printf ("ERROR: unnknown type of tree while differ, it is %d\n", tree->type);
        return nullptr;
    }
    }

#undef L
#undef R
#undef D
#undef C

#undef new_const
#undef new_op

DiffTree *copy_diff_tree (DiffTree *tree, DiffTree *new_parent)
    {
    assert (tree);

    DiffTree *copy = new_diff_tree_ctor (tree->type, new_parent, tree->data);
    
    if (tree->type == DT_VAR)
        copy->data.var = strdup (tree->data.var);

    if (tree->left)
        copy->left = copy_diff_tree (tree->left, copy);
    if (tree->right)
        copy->right = copy_diff_tree (tree->right, copy);

    return copy;
    }

DiffTreeData new_oper_data (DiffTreeOper oper)
    {
    DiffTreeData data;
    data.oper = oper;
    return data;
    }

#define L (tree->left)
#define R (tree->right)

// TODO do i need to create functions for this?
#define is_number(node, value) ( ((node)->type == DT_NUMBER) && is_same_double((node)->data.number, value) )
#define is_zero(node) ( is_number((node), 0) )
#define is_one(node) ( is_number((node), 1) )

int pass_to_parent_dt (DiffTree *tree, DiffTree *to_pass)
    {
    assert (tree);
    assert (to_pass);

    DEB ("I got to pass_to parent\n");          
                                                
    if (tree->parent->left == tree)             
        tree->parent->left = (to_pass);         
    else if (tree->parent->right == tree)       
        tree->parent->right = (to_pass);        
    else                                        
        {                                       
        printf ("Something went wrong, parent doesn't have tree, parent %p: ", tree->parent);  
        print_tree (tree->parent);              
        printf ("\n Parent's parent: ");        
        print_tree (tree->parent->parent);      
        printf ("\n Tree %p: ", tree);          
        print_tree (tree);                      
        printf ("\n");                          
        return 0;                               
        }                                       
                                                
    if (to_pass == R)                           
        diff_tree_dtor (L);                     
    else if (to_pass == L)                      
        diff_tree_dtor (R);                     
    else                                        
        printf ("You gave me strange thing to pass, it is %p, L is %p, R is %p\n", (to_pass), L, R);    
                                                
    to_pass->parent = tree->parent;             
                                                
    R = nullptr;                                
    L = nullptr;                                
    diff_tree_dtor (tree);                      
    return 1;                           
    }

#define pass_to_par(to_pass) {return pass_to_parent_dt (tree, to_pass);}

int simplify_diff_tree (DiffTree *tree)
    {
    assert (tree);

    int count = 0;
    int temp = 1;
    if (L)
        while (temp != 0)
            {
            temp = simplify_diff_tree (L);
            count += temp;
            }

    temp = 1; // is needed because of while
    if (R)
        while (temp != 0)
            {
            temp = simplify_diff_tree (R);
            count += temp;
            }
        
    calculate_diff_tree (tree, &count);

    do {
        temp = dt_calc_close (tree);
        count += temp;
    } while (temp);

    do {
        temp = remove_useless_nodes_dt (tree);
        count += temp;
    } while (temp);

    return count;
    }
    
int remove_useless_nodes_dt (DiffTree *tree)
    {
    assert (tree);

    if (tree->type == DT_OPERATOR)
        switch (tree->data.oper)
        {
        case DTO_PLUS:
        case DTO_MINUS:
            if (is_zero (L))
                pass_to_par (R);
            if (is_zero (R))
                pass_to_par (L);
            break;

        case DTO_MUL:
            if (is_one (L))
                pass_to_par (R);
            if (is_one (R))
                pass_to_par (L);
            break;

        case DTO_DIV:
            if (is_one (R))
                pass_to_par (L);
            break;

        case DTO_POW:
            if (is_one (R))
                pass_to_par (L);
            break;

        case DTO_LN:
        case DTO_SIN:
        case DTO_COS:
            break;
        
        default:
            printf ("ERROR: unknown operator while simplifying: %c\n", tree->data.oper);
            return 0;
        }

    return 0;
    }

#undef L
#undef R

#undef is_number
#undef is_zero
#undef is_one
#undef pass_to_par

#define L (calculate_diff_tree (tree->left))
#define R (calculate_diff_tree (tree->right))

double calculate_diff_tree (DiffTree *tree, int *count)
    {
    assert (tree);

    double value = NAN;
    switch (tree->type)
    {
    case DT_EXPRESSION:
        return calculate_diff_tree (tree->left);
    case DT_NUMBER:
        return tree->data.number;
    case DT_VAR:
        return NAN;

    case DT_OPERATOR:
        // TODO DSL to reduce code-repeat
        switch (tree->data.oper)
        {
#define PLUS(L_, R_) L_ + R_
#define MINUS(L_, R_) L_ - R_
#define MUL(L_, R_) L_ * R_
#define DIV(L_, R_) L_ / R_
#define POW(L_, R_) pow (L_, R_)
#define LN(L_, R_) log (R_)
#define SIN(L_, R_) sin (R_)
#define COS(L_, R_) cos (R_)


#define CALC_VAL(name_) case DTO_##name_: value = name_(L, R); break;
        CALC_VAL (PLUS)
        CALC_VAL (MINUS)
        CALC_VAL (MUL)
        CALC_VAL (DIV)
        CALC_VAL (POW)
        CALC_VAL (LN)
        CALC_VAL (SIN)
        CALC_VAL (COS)
#undef CALC_VAL

        default:
            printf ("ERROR: unknown operator while calculating: %c\n", tree->data.oper);
            value = NAN;
            break;
        }

        if (!isnan (value))
            {
            if (count)
                (*count)++;

            diff_tree_dtor (tree->left);
            tree->left = nullptr;
            diff_tree_dtor (tree->right);
            tree->right = nullptr;
            
            tree->type = DT_NUMBER;
            tree->data.number = value;
            }
        return value;

    default:
        printf ("ERROR: unknown type of tree while calculating: %d\n", tree->type);
        return NAN;
    }
    }

#undef L
#undef R

bool is_same_double (double a, double b)
    {
    return (fabs (a - b) < EPS);
    }

void diff_tree_dump (DiffTree *tree)
    {
    assert (tree);

    printf ("DUMP of tree, its parent is %p, it is type of %c and has data ", tree->parent, tree->type);
    switch (tree->type)
    {
    case DT_EXPRESSION:
        printf ("None\n");
        break;
    
    case DT_NUMBER:
        printf ("%g\n", tree->data.number);
        break;

    case DT_OPERATOR:
        printf ("%c\n", tree->data.oper);
        break;

    case DT_VAR:
        printf ("%s\n", tree->data.var);
        break;

    default:
        printf ("Error, unknown type of tree\n");
        break;
    }

    if (tree->left)
        {
        printf ("It has left child:\n");
        diff_tree_dump (tree->left);
        }

    if (tree->right)
        {
        printf ("It has right child:\n");
        diff_tree_dump (tree->right);
        }
    }

#define TELL_SELF do {                  \
    printf ("This happened at tree: "); \
    print_tree (tree);                  \
    printf ("\n\n");                    \
    } while (0)                     
    
bool check_diff_tree (DiffTree *tree)
    {
    assert (tree);

    switch (tree->type)
    {
    case DT_OPERATOR:
        switch (tree->data.oper)
        {
        case DTO_INVALID:
            printf ("Operator is DTO_invalid\n");
            TELL_SELF;
            return 0;

        case DTO_PLUS:
        case DTO_MINUS:
        case DTO_MUL:
        case DTO_DIV:
        case DTO_POW:
            if (!tree->left)
                {
                printf ("Binary operator %c doesn't have left, right is %p child\n", 
                        tree->data.oper, tree->right);
                TELL_SELF;
                return 0;
                }
            if (!tree->right)
                {
                printf ("Binary operator %c doesn't have right child, only left\n", 
                        tree->data.oper);
                TELL_SELF;
                return 0;
                }
            break;

        case DTO_LN:
        case DTO_SIN:
        case DTO_COS:
            if (tree->left)
                {
                printf ("Unary operator %c has left child\n", 
                        tree->data.oper);
                TELL_SELF;
                return 0;
                }
            if (!tree->right)
                {
                printf ("Unary operator %c doesn't have right child\n", 
                        tree->data.oper);
                TELL_SELF;
                return 0;
                }
            break;
        
        default:
            printf ("Unknown operator %d\n", tree->data.oper);
            TELL_SELF;
            return 0;
        }
        break;

    case DT_NUMBER:
        if (isnan (tree->data.number))
            {
            printf ("Number is NaN\n");
            TELL_SELF;
            return 0;
            }
        break;

    case DT_VAR:
        if (!tree->data.var)
            {
            printf ("Var name is NULL\n");
            TELL_SELF;
            return 0;
            }
        break;
    
    case DT_EXPRESSION:
        if (!tree->left)
            {
            printf ("Expression doesn't have expression\n");
            TELL_SELF;
            return 0;
            }
        if (tree->right)
            {
            printf ("Expression has two expressions\n");
            TELL_SELF;
            return 0;
            }
        break;

    default:
        printf ("Unknown type of tree\n");
        TELL_SELF;
        return 0;
    }

    if (tree->left)
        if (tree->left->parent != tree) 
            {
            printf ("Left child's parent is %p which is not actual parent %p\n", 
                    tree->left->parent, tree);            
            printf ("Left child is: ");
            print_tree (tree->left); 
            printf ("\nHis parent is "); 
            // print_tree (tree->left->parent); 
            printf ("\n"); 

            TELL_SELF;
            return 0;
            }

    if (tree->right)
        if (tree->right->parent != tree) 
            {
            printf ("right child's parent is %p which is not actual parent %p", 
                    tree->right->parent, tree);        
            printf ("right child is: ");
            print_tree (tree->right); 
            printf ("\nHis parent is "); 
            // print_tree (tree->right->parent); 
            printf ("\n"); 

            TELL_SELF;
            return 0;
            }
    
    if (tree->left && !check_diff_tree (tree->left))
        return 0;
    
    if (tree->right && !check_diff_tree (tree->right))
        return 0;

    return 1;    
    }

#undef TELL_SELF

#define is_number(node) ( (node)->type == DT_NUMBER )
#define is_any_num(node) ( (node)->left && (node)->left->type == DT_NUMBER || (node)->right && (node)->right->type == DT_NUMBER )

#define is_op(node, op) ( (node)->type == DT_OPERATOR && (node)->data.oper == (op) )

#define L tree->left
#define R tree->right

#define PLUS(a,b) ((a) + (b))
#define MUL(a,b) ((a) * (b))

// TODO move to function, idk why it is not already done
#define CHECK_CLOSE(op_name, op, frst, scnd)                        \
            if (is_number (frst))                                      \
                {                                                   \
                if (is_op (scnd, op_name) && is_any_num (scnd))          \
                    {                                               \
                    DiffTree *num = nullptr, *other = nullptr;      \
                    if (scnd->left->type == DT_NUMBER)                 \
                        {                                           \
                        num = scnd->left;                              \
                        other = scnd->right;                           \
                        scnd->right = nullptr;                      \
                        }                                           \
                    else                                            \
                        {                                           \
                        num = scnd->right;                             \
                        other = scnd->left;                            \
                        scnd->left = nullptr;                      \
                        }                                           \
                                                                    \
                    frst->data.number = op(frst->data.number, num->data.number);  \
                    diff_tree_dtor (scnd);                           \
                    scnd = other;                                      \
                    other->parent = tree;                           \
                                                                    \
                    return 1;                                        \
                    }                                               \
                } else do {} while(0)                                                   

int dt_calc_close (DiffTree *tree)
    {
    assert (tree);

    if (tree->type == DT_EXPRESSION)
        return dt_calc_close (tree->left);

    if (tree->type == DT_OPERATOR)
        {
        switch (tree->data.oper)
        {
        case DTO_INVALID:
            printf ("Operator is invalid\n");
            return 0;

        case DTO_PLUS:
            CHECK_CLOSE (DTO_PLUS, PLUS, L, R);
            CHECK_CLOSE (DTO_PLUS, PLUS, R, L);
            break;

        case DTO_MUL:
            CHECK_CLOSE (DTO_MUL, MUL, L, R);
            CHECK_CLOSE (DTO_MUL, MUL, R, L);
            break;

        case DTO_POW:
            if (is_number (R))
                if (is_op (L, DTO_POW) && is_number (L->right))
                    {
                    R->data.number *= L->right->data.number;
                    printf ("New number is %g\n", R->data.number);
                    DiffTree *tmp = L->left;
                    L->left = nullptr;
                    diff_tree_dtor (L);
                    L = tmp;
                    L->parent = tree;
                    }
            break;

        default:
            break;
        }
        }
    
    return 0;
    }


#undef is_number
#undef is_any_num
#undef is_op
#undef L 
#undef R 
#undef PLUS
#undef MUL
#undef CHECK_CLOSE

int oper_precedence (DiffTreeOper oper)  
    {
    switch (oper)
    {
    case DTO_PLUS:
    case DTO_MINUS:
        return 4;
    
    case DTO_MUL:
    case DTO_DIV:
        return 3;

    case DTO_POW:
        return 2;

    case DTO_LN:
    case DTO_SIN:
    case DTO_COS:
    case DTO_INVALID:
        // TODO do i need inf? they all are already magic numbers
        return 1<<30;

        return 0;

    default:
        printf ("Unknown operator %d precedence\n", oper);
        return 0;
    }
    }

void diff_tree_graph_node(const DiffTree *tree, FILE *gv_out)
    {
    assert(tree);
    assert(gv_out);

    fprintf(gv_out, "\t%lu[shape=record, label=\"", tree);

    switch (tree->type)
    {
    case DT_EXPRESSION:
        fprintf (gv_out, "None");
        break;
    
    case DT_NUMBER:
        fprintf (gv_out, "%g", tree->data.number);
        break;

    case DT_OPERATOR:
        fprintf (gv_out, "%c", tree->data.oper);
        break;

    case DT_VAR:
        fprintf (gv_out, "%s", tree->data.var);
        break;

    default:
        fprintf (gv_out, "Unknown\n");
        break;
    }
    
    fprintf (gv_out, "\"];\n");

    if (tree->left) 
        diff_tree_graph_node (tree->left, gv_out);
    if (tree->right) 
        diff_tree_graph_node (tree->right, gv_out);
    }

void diff_tree_graph_arrow(const DiffTree *tree, FILE *gv_out)
    {
    assert(tree);
    assert(gv_out);

    if (tree->left)
        fprintf(gv_out, "\t%lu -> %lu[fontsize=12]\n", tree, tree->left);

    if (tree->right)
        fprintf(gv_out, "\t%lu -> %lu[fontsize=12]\n", tree, tree->right);

    if (tree->left)
        diff_tree_graph_arrow (tree->left, gv_out);
    if (tree->right)
        diff_tree_graph_arrow (tree->right, gv_out);
    }

void diff_tree_graph_dump(const DiffTree *tree)
    {
    FILE *gv_out = fopen ("diff_tree.gv", "w");

    fprintf (gv_out, "# This is automatically generated dump of differentiation tree\n"
                    "digraph Tree{\n\n"
                    "\trankdir=UD;\n\n"
                    "\tnode[color=\"red\",fontsize=14];\n\n");

    diff_tree_graph_node (tree->left, gv_out);

    fprintf (gv_out, "\n");

    diff_tree_graph_arrow (tree->left, gv_out);

    fprintf (gv_out, "\n");

    fprintf (gv_out, "}");

    fclose (gv_out);

    system ("dot -Tpng diff_tree.gv -o diff_tree.png && diff_tree.png");
    }


