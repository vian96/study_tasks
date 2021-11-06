#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main () {
    List list = {};
    list_ctor (&list, 10);
    list_insert_ptr (&list, 0, 1);
    list_insert_ptr (&list, 1, 2);
    list_insert_ptr (&list, 2, 3);
    list_insert_ptr (&list, 1, 4);
    list_insert_ptr (&list, 1, 5);
    

    list_dump (&list);

    int t = get_list_data (&list, 0);
    printf ("%d\n", t);
    t = get_list_data (&list, 1);
    printf ("%d\n", t);
    t = get_list_data (&list, 2);
    printf ("%d\n", t);
    t = get_list_data (&list, 3);
    printf ("%d\n", t);
    t = get_list_data (&list, 4);
    printf ("%d\n", t);
    t = get_list_data (&list, 5);
    printf ("%d\n", t);

    return 0;
}



