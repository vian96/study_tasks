const double EPS = 10e-3;

enum DiffTreeType
    {
    DT_OPERATOR = 'O',
    DT_NUMBER = 'N',
    DT_VAR = 'V',
    DT_EXPRESSION = 'E', // it is used for changing children in parent
    };

enum DiffTreeOper
    {
    DTO_INVALID = 0,
    DTO_PLUS = '+',
    DTO_MINUS = '-',
    DTO_MUL = '*',
    DTO_DIV = '/',
    DTO_POW = '^',
    };

union DiffTreeData
    {
    double number;
    char *var;
    DiffTreeOper oper;
    };


struct DiffTree
    {
    DiffTreeType type;
    DiffTree *parent;
    DiffTreeData data;
    
    DiffTree *left, *right;
    };

void diff_tree_ctor (DiffTree *tree, DiffTreeType type, DiffTree *parent = nullptr, DiffTreeData data = {0}, DiffTree *left = nullptr, DiffTree *right = nullptr);

DiffTree *new_diff_tree_ctor (DiffTreeType type, DiffTree *parent = nullptr, DiffTreeData data = {0}, DiffTree *left = nullptr, DiffTree *right = nullptr);

void diff_tree_dtor (DiffTree *tree);

DiffTree *read_expression (const char **str, DiffTree *parent);

DiffTreeOper read_dt_oper (const char **str);

void print_tree (const DiffTree *tree);

void print_oper (DiffTreeOper oper);

void dt_to_latex (DiffTree *tree);

DiffTree *dt_differ (DiffTree *tree, DiffTree *parent = nullptr);

DiffTree *copy_diff_tree (DiffTree *tree, DiffTree *new_parent);

DiffTreeData new_oper_data (DiffTreeOper oper);

int simplify_diff_tree (DiffTree *tree);

double calculate_diff_tree (DiffTree *tree, int *count = nullptr);

bool is_same_double (double a, double b);

void diff_tree_dump (DiffTree *tree);

bool check_diff_tree (DiffTree *tree);

int dt_calc_close (DiffTree *tree);

int oper_precedence (DiffTreeOper oper);
