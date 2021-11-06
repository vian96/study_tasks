#include "list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void list_ctor (List *list, int capacity) {
    assert (list);
    assert (capacity); // TODO make it able to reallocate

    // capacity + 1 because the first element is always zero
    list->parts = (ListPart*) calloc (capacity + 1, sizeof (ListPart));
    list->parts[0] = {0, 0, 0};
    for (int i = 1; i <= capacity; i++)
        list->parts[i] = {0, i + 1, -1}; // next = i + 1 because it marks next free elem
    
    list->capacity = capacity;
    list->free = 1; // TODO if capacity is less than 2 then no free
    list->head = -1;
    list->tail = -1;
    list->size = 0;
    list->is_sorted = 1;
}

void list_dtor (List *list) {
    assert (list);
    
    free (list->parts);

    // TODO is it okay?
    memset (list, 0, sizeof (*list)); 
}

int list_check (List *list) {
    assert (list);
    
    // TODO check of list
    return 1;
}

// TODO output file
void list_dump (List *list) {
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

    for (int i = 0; i < list->capacity; i++) {
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

    /*
    printf ("\n------------- DUMP of free elems---------------\n");
    int ptr = list->free;
    int cnt = 0;
    int screen_len = 15;
    while (ptr != 0) {
        printf ("%d\n", 0);

        cnt++;
    }
    */

    printf ("\n-------------END OF DUMP---------------\n");
}

void list_insert_ptr (List *list, int ptr, ListDataT data) {
    assert (list);

    if (list->is_sorted) {
        // TODO check if it is sorted
    }

    if (ptr >= list->capacity) {
        printf ("You are exceeding borders of list\n");
        return;
    }

    ListPart *part = list->parts + ptr;
    int free_ptr = get_free_elem (list);
    
    if (free_ptr == 0 || free_ptr == -1) {
        // TODO reallocating list 
        printf ("Not enough allocated memory, returning\n"); 
        return;
    }

    if (ptr == 0) {
        if (list->size == 0) {
            list->parts[free_ptr].next = 0;
            list->tail = free_ptr;
        }
        else {
            list->parts[free_ptr].next = list->head;
            list->parts[list->head].prev = free_ptr;
        }
        
        list->parts[free_ptr].prev = 0;
        list->head = free_ptr;
    }
    else {
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
}

void list_remove_ptr (List *list, int ptr) {
    assert (list);
    assert (ptr);

    if (list->is_sorted) {
        // TODO check if it is sorted
    }

    ListPart *part = list->parts + ptr + 1;

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

ListDataT get_list_data (List *list, int index) {
    assert (list);

    ListPart *temp = get_list_elem (list, index);
    if (!temp)
        return 0;
    
    return temp->data;
}

ListPart *get_list_elem (List *list, int index) {
    assert (get_list_elem);

    if (index >= list->size) {
        printf ("Needed index is greater than size of list\n");
        return nullptr;
    }

    if (list->is_sorted) {
        // TODO check if it is sorted
    }

    int ptr = list->head;
    if (ptr == -1) {
        printf ("You're trying to access elements from empty list\n");
        return nullptr;
    }

    for (int i = 0; i < index; i++)
        ptr = list->parts[ptr].next;
    
    if (ptr == 0) {
        printf ("Some error, iterating list resulted into out of bounds\n");
        return nullptr;
    }

    return list->parts + ptr;
}

int get_free_elem (List *list) {
    assert (list);

    printf ("Free element is %d\n", list->free);

    if (list->free == 0 || list->free == -1) {
        printf ("No free place, error, returning nothing\n");
        return -1;
    }

    int temp = list->free;
    list->free = list->parts[temp].next;

    return temp;
}



