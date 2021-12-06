#ifndef AKINATOR_AKINATOR_H
#define AKINATOR_AKINATOR_H

#include "akin_tree.h"

void play_akin (AkinTree *tree);

void clear_input_buffer ();

char ask_yes_no (const char *message);

#endif // AKINATOR_AKINATOR_H
