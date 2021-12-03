#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int test1 ();
int test2 ();
int test3 ();
int test4 ();
int test5 ();

int main () 
    {
    assert (test1 ());
    assert (test2 ());
    assert (test3 ());
    assert (test4 ()); 
    assert (test5 ());
    
    return 0;
    }

int test1 () 
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

    list_dump (lst);
    //print_list_elems (lst);

    list_remove_ptr (lst, 2);
    list_remove_ptr (lst, 3);

    list_dump (lst);
    int is_ok = list_check (lst);
    print_list_elems (lst);
    list_dtor (lst);

    return is_ok;
    }

int test2 () 
    {
    List list = {};
    List *lst = &list;
    list_ctor (lst, 0);

    for (int i = 0; i < 20; i++)
        list_insert_ptr (lst, 0, 13 + i);
    
    list_dump (lst);
    int is_ok = list_check (lst);
    list_dtor (lst);

    return is_ok;
    }

int test3 ()
    {
    List list = {};
    List *lst = &list;
    list_ctor (lst, 0);
    int temp = 0;

    printf ("Constructed\n");
    for (int i = 0; i < 15; i++)
        temp = list_insert_ptr (lst, temp, i - 7);
    
    list_dump (lst);
    int is_ok = list_check (lst);
    list_dtor (lst);

    return is_ok;
    }

int test4 ()
    {
    List list = {};
    List *lst = &list;
    list_ctor (lst, 0);
    int temp = 0;

    for (int i = 0; i < 15; i++)
        temp = list_insert_ptr (lst, temp, i - 7);

    for (int i = 0; i < 15; i++)
        list_remove_ptr (lst, 1 + i);


    list_dump (lst);
    int is_ok = list_check (lst);
    list_dtor (lst);

    return is_ok;
    }

int test5 () 
    {
    List list = {};
    List *lst = &list;
    list_ctor (lst, 10);

    list_insert_ptr (lst, 0, 1);
    list_insert_ptr (lst, 1, 2);
    list_insert_ptr (lst, 2, 3);
    list_insert_ptr (lst, 3, 4);
    list_insert_ptr (lst, 0, 5);

    list_remove_ptr (lst, 2);
    list_remove_ptr (lst, 3);

    linearize_list (lst);
    list_dump (lst);
    int is_ok = list_check (lst);
    print_list_elems (lst);
    list_dtor (lst);

    return is_ok;
    }

