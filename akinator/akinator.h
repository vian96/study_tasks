#ifndef AKINATOR_AKINATOR_H
#define AKINATOR_AKINATOR_H

#include "akin_tree.h"

#include "../stack/source/stack.h"

void play_akin (AkinTree *tree);

void clear_input_buffer ();

char ask_yes_no (const char *message);

void akin_seek_definition (AkinTree *tree, const char *name, Stack *stk_yes, Stack *stk_no);

#endif // AKINATOR_AKINATOR_H
