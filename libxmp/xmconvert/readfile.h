#ifndef readfile_h_seen
#define readfile_h_seen

#include <stdio.h>
#include <stdlib.h>

#define ASSERT(cond) \
{\
  if (!(cond)) {\
    fprintf(stderr, "ASSERT FAILED: %s\n%s:%d\n\n",#cond,__FILE__,__LINE__); \
    exit(-1);\
  }\
}

/* layout of data in rom... */

typedef struct {
    unsigned int length;
    unsigned int loop_start;
    unsigned int loop_length;
    signed char relative;   /* needed? this is == finetune? */
    signed char finetune;   /* needed? this is == finetune? */
    unsigned char vol;
    unsigned char pan;
    char *data;             /* pointer to data */
    unsigned short volume_fade;
    unsigned short venv_pts[24];
    unsigned char num_venv_pts;
    unsigned char venv_sust_pt;
    unsigned char venv_loop_start;
    unsigned char venv_loop_end;
    unsigned char venv_type;
    unsigned int checksum;  /* not in rom */
} sample_t;

typedef struct {
    unsigned char note;     /* convert from odd note format to some index into a LUT */
    unsigned char sample;
    unsigned char volume;
    unsigned char effect;
    unsigned char param;    /* effect param */
} pattern_t;

/* e.g. */
/* pattern data is stored as u8  (in a .s file) */
/* sample data is stored as u8   (in a .s file) */
/* order table is stored as u8 e.g. {0,1,2,3,4,5,6}; */

/* the pattern here is u8* pattern_table[]= {pattern0, pattern1, etc}; */
/*  */
/* the order is the table above */
/* the sampe is a table:   sample_t samples[] = { */
/*    {1000,600,400, 8363,64, sample_0 }, */
/*    etc */
/* }; */
/*  */
/*  */

/* will also need: */

typedef struct {
    unsigned char rows; /* 64 for mod, variable for XM */
    int index;      /* index of the pattern data (converted to "pointer" name) */
} pattern_header_t;

typedef struct {
    sample_t *sample;
    unsigned char *order;   /* i.e. order[1] */
    pattern_t **pattern;    /* i.e. pattern[order[1]][row].note */
    unsigned short song_length;
    unsigned short restart;
    unsigned char speed;    /* ticks per row (Fxx xx < 0x20) */
    unsigned char tempo;    /* BPM           (Fxx xx >= 0x20) */
    unsigned char channels; /* BPM           (Fxx xx >= 0x20) */
} mod_t;

typedef struct {
    int verbose;
    int use_unsigned;
    int use_underscore;
    int inc_envelope;
    int file_index;
    char **song_titles;
} globals_t;

extern globals_t globals;

#endif
