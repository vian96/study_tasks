#include "../../onegin/file_utils.h"

void prepare_file_text (FileText *code);

char *create_out_name (const char *in_name);

char *parse_commands (const FileText *code, size_t *out_size);

size_t get_out_size (const FileText *code);

size_t create_out_arr (const FileText *code, char *out);
