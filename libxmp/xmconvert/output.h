#ifndef output_h_seen
#define output_h_seen

#include "readfile.h"
#include <stdio.h>

void output_init_s(FILE *);
void output_init_c(FILE *);

void output_order_table(int, unsigned char *, FILE *);
void output_patterns(int, pattern_t *, int, int, FILE *);
void output_sample(int, char *, int, FILE *);

void output_pattern_headers(pattern_header_t *, int, FILE *);

void output_start_sample_table(FILE *);
void output_sample_table(int, int, sample_t *, FILE *);
void output_end_sample_table(FILE *);

void output_mod_header(mod_t *, FILE *);
void output_module_table(FILE *output_c);
void output_modules_header(FILE *output_h);
#endif
