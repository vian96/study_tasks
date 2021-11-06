#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main () 
    {
    List list = {};
    List *lst = &list;
    list_ctor (lst, 10);
    
    //list_dump (lst);
    printf ("Check of list: %d\n", list_check (lst));

    list_insert_ptr (lst, 0, 1);
    list_insert_ptr (lst, 1, 2);
    list_insert_ptr (lst, 2, 3);
    list_insert_ptr (lst, 3, 4);
    list_insert_ptr (lst, 0, 5);

    //list_dump (lst);
    printf ("Check of list: %d\n", list_check (lst));
    print_list_elems (lst);

    list_remove_ptr (lst, 5);
    list_remove_ptr (lst, 4);

    //list_dump (lst);
    printf ("Check of list: %d\n", list_check (lst));
    print_list_elems (lst);

    list_dtor (lst);

    return 0;
    }



