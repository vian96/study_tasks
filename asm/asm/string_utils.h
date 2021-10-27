#ifndef ASM_STRING_UTILS_H
#define ASM_STRING_UTILS_H

#include "../config.h"

#include "../../onegin/file_utils.h"

bool is_empty_string (const char *str);

void skip_blank (const char **str);

void skip_not_blank (const char **str);

void skip_alnum (const char **str);

void skip_name (const char **str);

void str_to_upper (char *str);

bool is_in_name (int c);

void str_to_upper (char *str);

void prepare_file_text (FileText *code);

bool is_equal_words (const char *a, const char *b);

#endif // ASM_STRING_UTILS_H

