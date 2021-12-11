#ifndef AKINATOR_AKINATOR_H
#define AKINATOR_AKINATOR_H

#include "akin_tree.h"

#include "../stack/source/stack.h"

void akinator_app ();

void play_akin (AkinTree *tree);

void clear_input_buffer ();

char ask_yes_no (const char *message);

void akin_seek_def (AkinTree *tree, const char *name, Stack *stk_names = nullptr, Stack *stk_is = nullptr);

int akin_seek_def_stack (AkinTree *tree, const char *name, Stack *stk_names, Stack *stk_is);

void akin_diff_def (AkinTree *tree, const char *name1, const char *name2);

#endif // AKINATOR_AKINATOR_H
