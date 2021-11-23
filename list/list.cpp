#include "list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// TODO deal with capacity and capacity + 1, i am very puzzled

static int max_int (int a, int b) {
    return (a > b) ? a : b;
}

void list_ctor (List *list, int capacity) 
    {
    assert (list);
    assert (capacity > 1); // TODO make it able to reallocate

    list->parts = (ListPart*) calloc (capacity, sizeof (ListPart));
    list->parts[0] = {0, 0, 0};
    for (int i = 1; i < capacity; i++)
        list->parts[i] = {0, i + 1, -1}; // next = i + 1 because it marks next free elem

    list->parts[capacity - 1].next = 0;
    
    list->capacity = capacity;
    list->free = 1; // TODO if capacity is less than 2 then no free
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
        return 0;
    
    int ptr = list->head;
    if (ptr == -1)
        {
        if (list->tail != -1)
            return 0;
        if (list->size != 0)
            return 0;
        }
    
    if (list->is_sorted == 1) 
        {
        // TODO check is sorted
        }

    if (list->capacity)
        if (list->parts->data != 0 || list->parts->next != 0 || list->parts->prev != 0) 
            {
            printf (
                    "Error, list data at null index is corrupted, got:\n"
                    "data: %d, next: %d, prev: %d\nExpected only zeroes\n",
                    list->parts->data, list->parts->next, list->parts->prev
                );
            return 0;
            }
    
    int cnt = 0; // number of counted elems of list

    if (ptr != -1) 
        {
        ListPart *part = list->parts + ptr, *next = nullptr;
        int prev = ptr;
        cnt++; // because it will miss one element when going through loop
        while (part->next != 0) 
            {
            next = list->parts + part->next;
            if (next->prev != prev) 
                {
                printf (
                    "Error, elem's %d prev %d doesn't point to actual previous elem %d\n", 
                    part->next, next->prev, prev
                );
                return 0;
                }
            prev = part->next;
            part = next;
            cnt++;
            }
        
        if (list->tail != part - list->parts) 
            {
            printf (
                "Error, tail %d doesn't point to actual end of list %d\n", 
                list->tail, part - list->parts
            );
            return 0;
            }

        if (cnt != list->size) 
            {
            printf (
                "Error, list's size %d is not the same as actual size %d\n", 
                list->size, cnt
            );
            return 0;
            }
        }

    if (list->free != -1 && list->free != 0) 
        {
        ListPart *part = list->parts + list->free;
        cnt++; // because it will miss one element when going through loop
        while (part->next != 0) 
            {
            if (part->prev != -1) 
                {
                printf (
                    "Error, elem with index %d is considered to be free, "
                    "however prev is %d but not -1\n",
                    part - list->parts, part->prev
                );
                return 0;
                }
            part = list->parts + part->next;
            cnt++;
            }
        }
        
    if (list->capacity && cnt + 1 != list->capacity) 
        {
        printf (
            "I counted %d elems (not including nul indexed one), but it should have %d, "
            "you missed some of them\n", 
            cnt, list->capacity
        );
        return 0;
        }

    return 1;
    }

// TODO output file
void list_dump (const List *list) 
    {
    printf ("\n-------------LIST DUMP---------------\n");

    printf (
        "head: %d\n"
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
        list->capacity
    );

    for (int i = 0; i < list->capacity; i++) 
        {
        ListPart *part = list->parts + i;
        printf (
            "index: %d \t"
            "data: %d \t"
            "next: %d \t"
            "prev: %d \t\n",
            i,
            part->data,
            part->next,
            part->prev
        );
        }

    printf ("\n-------------END OF LIST DUMP---------------\n\n\n");
    
    printf ("Creating graphviz dump...\n");

    FILE *gv_out = fopen ("list_dump.gv", "w");
    // TODO change fprintf to fputs when not needed
    fprintf (
        gv_out,
        "# This is automatically generated dump of list\n"
        "digraph D\n"
        "    {\n"
        "    rankdir=LR;\n"
        "    node [shape=record];\n\n\n"
    );

    fprintf (gv_out, "    node0[label = \"index:0 | data:0 | next:0 | prev:0\"];\n\n");
    for (int i = 1; i < list->capacity; i++) 
        {
        ListPart *cur = list->parts + i;
        fprintf (
            gv_out,
            "    node%d[shape=record label = "
            "\"<i> index : %d | <d> data : %d | <n> next : %d | <p> prev : %d\"];\n",
            i, i, cur->data, cur->next, cur->prev
        );
        if (cur->next != -1)
            fprintf (
                gv_out, 
                "    node%d:<n> -> node%d [color=red constraint=false];\n",
                i, cur->next
            );
        if (cur->prev != -1)
            fprintf (
                gv_out, 
                "    node%d:<p> -> node%d [color=green constraint=false];\n\n",
                i, cur->prev
            );
        }

    fprintf (gv_out, "        {\n        edge[color=white]\n");
    for (int i = 0; i + 1 < list->capacity; i++) 
        fprintf (gv_out, "        node%d -> node%d\n", i, i + 1);

    fprintf (gv_out, "        }\n");


    fprintf (gv_out, "\n    }\n");
    fclose (gv_out);
    printf ("End of graphviz dump...\n");
    }

int list_insert_ptr (List *list, int ptr, ListDataT data) 
    {
    assert (list);

    if (list->is_sorted) 
        {
        // TODO check if it is sorted
        }

    if (ptr >= list->capacity) 
        {
        printf ("You are exceeding borders of list\n");
        return 0;
        }

    ListPart *part = list->parts + ptr;
    if (part->prev == -1) 
        {
        printf ("You're trying to put elem after non-existing part of list\n");
        return 0;
        }

    int free_ptr = get_free_elem (list);    
    if (free_ptr == 0 || free_ptr == -1) 
        {
        // TODO reallocating list 
        printf ("Not enough allocated memory, returning\n"); 
        return 0;
        }

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

    return free_ptr;
    }

void list_remove_ptr (List *list, int ptr) 
    {
    assert (list);
    assert (ptr);

    if (ptr >= list->capacity) 
        {
        printf ("You are exceeding borders of list\n");
        return;
        }

    if (list->is_sorted) 
        {
        // TODO check if it is sorted
        }

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
        {
        // TODO check if it is sorted
        }

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

    // TODO maybe add debug option?
    // printf ("Free element is %d\n", list->free);

    if (list->free == 0 || list->free == -1) 
        {
        printf ("No free place, error, returning nothing\n");
        return -1;
        }

    int temp = list->free;
    list->free = list->parts[temp].next;

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




