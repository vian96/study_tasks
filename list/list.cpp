#include "list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef NDEBUG
#define DEB(...) printf(__VA_ARGS__)
#else
#define DEB(...) 
#endif

static int max_int (int a, int b) {
    return (a > b) ? a : b;
}

void list_ctor (List *list, int capacity) 
    {
    assert (list);

    capacity = max_int (capacity, MIN_LIST_CAP);
    list->parts = (ListPart*) calloc (capacity, sizeof (ListPart));
    list->parts[0] = {0, 0, 0};
    for (int i = 1; i < capacity; i++)
        list->parts[i] = {0, i + 1, -1}; // next = i + 1 because it marks next free elem

    list->parts[capacity - 1].next = 0;
    
    list->capacity = capacity;
    list->free = 1; 
    list->head = -1;
    list->tail = -1;
    list->size = 0;
    list->is_sorted = 1;
    }

void list_dtor (List *list) 
    {
    assert (list);
    
    free (list->parts);

    // TODO is it okay?
    memset (list, 0, sizeof (*list)); 
    }

int list_check (const List *list) 
    {
    assert (list);
    
    if (!(list->size == 0 && list->capacity == 0) && 
         (list->size >= list->capacity || list->size < 0 || list->capacity < 0)) 
        {
        printf ("Some weird error with numbers\n");
        return 0;
        }
    
    int ptr = list->head;
    if (ptr == -1 || ptr == 0)
        {
        if (list->tail != -1 && list->tail != 0)
            {
            printf ("Rrror with tail %d\n", list->tail);
            return 0;
            }
        if (list->size != 0)
            {
            printf ("Rrror with size\n");
            return 0;
            }
        }
    
    if (list->capacity)
        if (list->parts->data != 0 || list->parts->next != 0 || list->parts->prev != 0) 
            {
            printf ("Error, list data at null index is corrupted, got:\n"
                    "data: %d, next: %d, prev: %d\nExpected only zeroes\n",
                list->parts->data, list->parts->next, list->parts->prev);
            return 0;
            }

    DEB ("Checked easy info\n");
    
    if (list->is_sorted == 1) 
        {
        for (int i = 1; i < list->head; i++) 
            if (list->parts[i].prev != -1)
                {
                printf ("Expected linearized list, but i have met non-empty element "
                        "at pos %d before begin of list at post %d\n",
                    i, list->head);
                return 0;
                }

        for (int i = list->tail + 1; i < list->capacity; i++) 
            if (list->parts[i].prev != -1)
                {
                printf ("Expected linearized list, but i have met non-empty element "
                        "at pos %d after end of list at post %d\n",
                    i, list->tail);
                return 0;
                }

        for (int i = list->head; i <= list->tail; i++) 
            {
            ListPart *part = list->parts + i;
            if (part->prev != i - 1 && (part->prev != 0 || i != list->head) || 
                part->next != i + 1 && (part->next != 0 || i != list->tail))
                {
                printf ("Wrong part of list at pos %d\n"
                        "Got values: prev: %d, next: %d,\n"
                        "Expected: prev: %d, next: %d "
                        "(or maybe 0 if it is at border of a list)\n",
                        i, part->prev, part->next,
                        i - 1, i + 1);
                return 0;
                }
            }

        }

    DEB ("Checked linearised\n");
    
    int cnt = 0; // number of counted elems of list

    if (ptr != -1 && ptr != 0) 
        {
        ListPart *part = list->parts + ptr, *next = nullptr;
        int prev = ptr;
        cnt++; // because it will miss one element when going through loop
        while (part->next != 0) 
            {
            next = list->parts + part->next;
            if (next->prev != prev) 
                {
                printf ("Error, elem's %d prev %d doesn't point to actual previous elem %d\n", 
                        part->next, next->prev, prev);
                return 0;
                }
            prev = part->next;
            part = next;
            cnt++;
            }
        
        if (list->tail != part - list->parts) 
            {
            printf ("Error, tail %d doesn't point to actual end of list %d\n", 
                    list->tail, part - list->parts);
            return 0;
            }

        if (cnt != list->size) 
            {
            printf ("Error, list's size %d is not the same as actual size %d\n", 
                    list->size, cnt);
            return 0;
            }
        }

    DEB ("Checked size info\n");

    if (list->free != -1 && list->free != 0) 
        {
        if (list->free >= list->capacity)
            {
            printf ("Some error with free it is %d and is bigger than capacity %d\n", 
                    list->free, list->capacity);
            return 0;
            }
        ListPart *part = list->parts + list->free;
        cnt++; // because it will miss one element when going through loop
        while (part->next != 0) 
            {
            if (part->prev != -1) 
                {
                printf ("Error, elem with index %d is considered to be free, "
                        "however prev is %d but not -1\n",
                        part - list->parts, part->prev);
                return 0;
                }
            part = list->parts + part->next;
            cnt++;
            }
        }
        
    DEB ("Checked free\n");
    
    if (list->capacity && cnt + 1 != list->capacity) 
        {
        printf ("I counted %d elems (not including null indexed one), but it should have %d, "
                "you missed some of them\n", 
                cnt, list->capacity);
        return 0;
        }

    DEB ("Checked all\n"); 

    return 1;
    }

// TODO output file
void list_dump (const List *list) 
    {
    if (list == nullptr) 
        {
        printf ("-----------DUMP OF LIST ALLOCAED AT NULL PTR---------------------\n"
                "-----------END OF DUMP OF LIST ALLOCATED AT NULL PTR-------------\n\n");
        return;
        }
    
    printf ("\n-------------LIST DUMP---------------\n");

    printf ("head: %d\n"
            "tail: %d\n"
            "free: %d\n"
            "is sorted: %d\n"
            "size: %d\n"
            "capacity: %d\n\n",
            list->head,
            list->tail,
            list->free,
            list->is_sorted,
            list->size,
            list->capacity);

    if (list_check (list))
        printf ("LIST IS OK!\n");
    else
        printf ("LIST IS BAD!!!!\n");

    for (int i = 0; i < list->capacity; i++) 
        {
        ListPart *part = list->parts + i;
        printf ("index: %d \t"
                "data: %d \t"
                "next: %d \t"
                "prev: %d \t\n",
                i,
                part->data,
                part->next,
                part->prev);
        }
    
    DEB ("Creating file for graphviz\n");
    FILE *gv_out = fopen ("list_dump.gv", "w+");
    DEB ("Opened graphviz file\n");

    fprintf (gv_out,
            "# This is automatically generated dump of list\n"
            "digraph D\n"
            "    {\n"
            "    rankdir=LR;\n"
            "    node [shape=record];\n\n\n");

    fprintf (gv_out,
            "    node0:<head> -> node%d [constraint=false];\n"
            "    node0:<tail> -> node%d [constraint=false];\n"
            "    node0:<free> -> node%d [constraint=false];\n",
            list->head, list->tail, list->free);

    fprintf (gv_out, 
            "    node0[label = \"<head> head : %d | <tail> tail : %d | <free> free : %d | | "
                "index : 0 | data : 0 | next : 0 | prev : 0 | | "
                "size : %d | is sorted : %d | is ok : %d\"];\n\n",
            list->head, list->tail, list->free, list->size, list->is_sorted, list_check (list));
    
    DEB ("Put standard info\n");

    for (int i = 1; i < list->capacity; i++) 
        {
        ListPart *cur = list->parts + i;
        fprintf (gv_out,
                "    node%d[shape=record label = "
                "\"<i> index : %d | <d> data : %d | <n> next : %d | <p> prev : %d\"];\n",
                i, i, cur->data, cur->next, cur->prev);

        if (cur->next != -1)
            fprintf (gv_out, 
                    "    node%d:<n> -> node%d [color=red constraint=false];\n",
                    i, cur->next);

        if (cur->prev != -1)
            fprintf (gv_out, 
                    "    node%d:<p> -> node%d [color=green constraint=false];\n\n",
                    i, cur->prev);
        }

    DEB ("Created nodes\n");

    fprintf (gv_out, "        {\n        edge[color=white]\n");
    for (int i = 0; i + 1 < list->capacity; i++) 
        fprintf (gv_out, "        node%d -> node%d\n", i, i + 1);

    fprintf (gv_out, "        }\n");


    fprintf (gv_out, "\n    }\n");
    fclose (gv_out);
    DEB ("Closed graphviz file\n");

    system ("run_dot.bat");
    printf ("End of graphviz dump...\n");

    printf ("\n-------------END OF LIST DUMP---------------\n\n\n");
    }

int list_insert_ptr (List *list, int ptr, ListDataT data) 
    {
    assert (list);
    assert (ptr >= 0);
    assert (ptr < list->capacity);

    ListPart *part = list->parts + ptr;
    if (part->prev == -1) 
        {
        printf ("You're trying to put elem after non-existing part of list\n");
        return 0;
        }

    int free_ptr = get_free_elem (list);    
    if (free_ptr == 0 || free_ptr == -1) 
        {
        printf ("Not enough memory, returning\n"); 
        return 0;
        }
    part = list->parts + ptr; // DO NOT DELETE IT IN ANY POSSIBLE WAY, it is VERY IMPORTANT because of realloc

    if (ptr == 0) 
        {
        if (list->size == 0) 
            {
            list->parts[free_ptr].next = 0;
            list->tail = free_ptr;
            }
        else 
            {
            list->parts[free_ptr].next = list->head;
            list->parts[list->head].prev = free_ptr;
            }
        
        list->parts[free_ptr].prev = 0;
        list->head = free_ptr;
        }
    else 
        {
        if (part->next != 0)
            list->parts[part->next].prev = free_ptr;

        list->parts[free_ptr].next = part->next;
        list->parts[free_ptr].prev = ptr;
        part->next = free_ptr;
        if (list->tail == ptr)
            list->tail = free_ptr;
        }

    list->parts[free_ptr].data = data;
    list->size++;

    if (!(ptr == list->tail || ptr == 0 && (list->head == 0 || list->head == -1))) 
        list->is_sorted = 0;

    return free_ptr;
    }

void list_remove_ptr (List *list, int ptr) 
    {
    assert (list);
    assert (ptr > 0 && ptr < list->capacity);

    if (list->is_sorted) 
        if (ptr == list->tail) 
            {
            list->parts[ptr] = {0, ptr + 1, -1};
            list->parts[ptr - 1].next = 0;
            list->free = ptr;
            list->tail--;
            list->size--;
            return;
            }
    list->is_sorted = 0;

    ListPart *part = list->parts + ptr;

    if (part->prev == -1) 
        {
        printf ("You're trying to remove empty elem of list, returning\n");
        return;
        }

    if (part->next != 0)
        list->parts[part->next].prev = part->prev;
    if (part->prev != 0)
        list->parts[part->prev].next = part->next;

    // TODO see if list starts to be empty after removing
    if (list->head == ptr) 
        list->head = part->next;
    if (list->tail == ptr)
        list->tail = part->prev;

    // marks that this place is free
    part->prev = -1;
    // adds this place to list of free places
    part->next = list->free;
    list->free = ptr;

    list->size--;
    }

ListDataT get_list_data (const List *list, int index) 
    {
    assert (list);

    ListPart *temp = get_list_elem (list, index);
    if (!temp)
        return 0;
    
    return temp->data;
    }

ListPart *get_list_elem (const List *list, int index) 
    {
    assert (list);

    if (index >= list->size) 
        {
        printf ("Needed index is greater than size of list\n");
        return nullptr;
        }

    if (list->is_sorted)
        return list->parts + list->head + index;

    int ptr = list->head;
    if (ptr == -1) 
        {
        printf ("You're trying to access elements from empty list\n");
        return nullptr;
        }

    for (int i = 0; i < index; i++)
        ptr = list->parts[ptr].next;
    
    if (ptr == 0) 
        {
        printf ("Some error, iterating list resulted into out of bounds\n");
        return nullptr;
        }

    return list->parts + ptr;
    }

int get_free_elem (List *list) 
    {
    assert (list);

    DEB ("Starting free\n");
    if (list->free == 0 || list->free == -1) 
        {
        ListPart *tmp = (ListPart*) realloc (list->parts, list->capacity * 2 * sizeof (ListPart));
        if (tmp == nullptr) 
            {
            printf ("No memory while calling calloc\n");
            return 0;
            }
        DEB ("Got free\n");
        
        list->parts = tmp;
        for (int i = list->capacity; i < list->capacity * 2; i++)
            list->parts[i] = {0, i + 1, -1}; // next = i + 1 because it marks next free elem
            
        DEB ("Set free\n");
        
        list->parts[list->capacity * 2 - 1].next = 0;
        list->free = list->capacity;
        list->capacity *= 2;
        }

    int temp = list->free;
    list->free = list->parts[temp].next;
    DEB ("Ret free\n");

    return temp;
    }

// this function is uneffective because it is for testing
void print_list_elems (const List *list) 
    {
    assert (list);

    if (list->head == -1 || list->head == 0) 
        {
        printf ("List is empty\n");
        return;
        }
    
    printf ("Printing elements of list\n");
    for (int i = 0; i < list->size; i++)
        printf ("Elem %d is %d\n", i, get_list_data (list, i));

    printf ("\n");
    }

void linearize_list (List *list)
    {
    assert (list);

    ListPart *buff = (ListPart*) calloc (list->capacity, sizeof (ListPart));
    for (int i = 1; i < list->capacity; i++)
        buff[i] = {0, i + 1, -1};
    buff[list->capacity - 1] = {0, 0, -1};

    int ptr = list->head, ind = 1;
    while (ptr != 0)
        {
        buff[ind].data = list->parts[ptr].data;
        buff[ind].prev = ind - 1;
        ptr = list->parts[ptr].next;
        ind++;
        }
    buff[ind - 1].next = 0;

    free (list->parts);
    list->parts = buff;
    list->free = ind;
    list->head = 1;
    list->is_sorted = 1;
    list->tail = ind - 1;
    }


