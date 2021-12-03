
typedef int ListDataT;

const int MIN_LIST_CAP = 10;

struct ListPart 
    {
    ListDataT data;
    int next;
    int prev;
    };

struct List 
    {
    ListPart *parts;

    int head;
    int tail;
    int free;
    int is_sorted;
    int size;
    
    // capacity means maximum count of elements to be placed in list, 
    // so allocated size is capacity + 1 for including the first zero element
    int capacity; 
    };

void list_ctor (List *list, int capacity);

void list_dtor (List *list);

int list_check (const List *list);

void list_dump (const List *list);

// on success returns ptr to next elem
// on failure returns 0
int list_insert_ptr (List *list, int ptr, ListDataT data);

void list_remove_ptr (List *list, int ptr);

ListDataT get_list_data (const List *list, int index);

ListPart *get_list_elem (const List *list, int index);

int get_free_elem (List *list);

// this function is uneffective because it is for testing
void print_list_elems (const List *list);

void linearize_list (List *list);

