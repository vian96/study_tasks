
typedef int ListDataT;

// TODO change name
struct ListPart {
    ListDataT data;
    int next;
    int prev;
};

struct List {
    // TODO change name 
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

int list_check (List *list);

// TODO output file
void list_dump (List *list);

void list_insert_ptr (List *list, int ptr, ListDataT data);

void list_remove_ptr (List *list, int ptr);

ListDataT get_list_data (List *list, int index);

ListPart *get_list_elem (List *list, int index);

int get_free_elem (List *list);
