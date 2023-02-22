#include "output.h"

void output_init_c(FILE *output_c)
{
        /* write any includes, etc that need to come first */
        fprintf(output_c, "#include <xmp.h>\n");
        fflush(output_c);
}

void output_init_s(FILE *output_s)
{
        /* write any initialisation code, banners etc */
        fprintf(output_s, ".section .rodata\n");
        fflush(output_s);
}

void output_byte(FILE *output_s, int thebyte, int total)
{
        if ((total % 16) == 0) {
                fprintf(output_s, "\n.byte ");
        } else {
                fprintf(output_s, ",");
        }
        fprintf(output_s, "%d", thebyte);
}

void output_patterns(int index,
                     pattern_t *pattern_data,
                     int rows, int channels, FILE *output_s)
{
        /* output pattern data 'index' */
        /* use globals.file_index to tag all patterns */
        int length = rows * channels;
        /* pack the data using xm/s3m mix */
        /* byte 0 means row end */
        /* byte &0x80 means "something follows" */
        /* !&0x80 means "5 unpacked bytes follow" */
        fprintf(output_s, ".align 2\n");
        if (globals.use_underscore)
                fprintf(output_s, ".global _roXm%dPattern%d\n", globals.file_index, index);
        else
                fprintf(output_s, ".global roXm%dPattern%d\n", globals.file_index, index);
        if (globals.use_underscore)
                fprintf(output_s, "_roXm%dPattern%d :", globals.file_index, index);
        else
                fprintf(output_s, "roXm%dPattern%d :", globals.file_index, index);
        int row, ch;
        int total = 0;
        for (row = 0; row < rows; row++) {
                int last_channel = -1;
                for (ch = channels - 1; ch >= 0; ch--) {
                        int i = ch + row * channels;
                        if (pattern_data[i].note ||
                            pattern_data[i].sample ||
                            pattern_data[i].volume ||
                            pattern_data[i].effect ||
                            pattern_data[i].param) {
                                last_channel = ch;
                                break;
                        }
                }
                if (last_channel == -1) {
                        /* printf("Empty row, ouput 0\n"); */
                        output_byte(output_s, 0, total++);
                } else {
                        /* printf("last channel %d\n",last_channel); */
                        /* row has stuff in it */
                        for (ch = 0; ch <= last_channel; ch++) {
                                /* pack bytes */
                                int i = ch + row * channels;
                                if (pattern_data[i].note &&
                                    pattern_data[i].sample &&
                                    pattern_data[i].volume &&
                                    pattern_data[i].effect) {
                                        /* have to output the lot */
                                        /* printf("ouput the 5 bytes...\n"); */
                                        output_byte(output_s,
                                                    pattern_data[i].note,
                                                    total++);
                                        output_byte(output_s,
                                                    pattern_data[i].sample,
                                                    total++);
                                        output_byte(output_s,
                                                    pattern_data[i].volume,
                                                    total++);
                                        output_byte(output_s,
                                                    pattern_data[i].effect,
                                                    total++);
                                        output_byte(output_s,
                                                    pattern_data[i].param,
                                                    total++);
                                } else {
                                        unsigned char what = 0x80;
                                        if (pattern_data[i].note) {
                                                what |= 1;
                                        }
                                        if (pattern_data[i].sample) {
                                                what |= 2;
                                        }
                                        if (pattern_data[i].volume) {
                                                what |= 4;
                                        }
                                        if (pattern_data[i].effect
                                            || pattern_data[i].param) {
                                                what |= 8;
                                        }
                                        /* printf("what %02x ", what); */
                                        output_byte(output_s, what,
                                                    total++);
                                        if (what & 1) {
                                                /* printf("note %02x ", pattern_data[i].note); */
                                                output_byte(output_s,
                                                            pattern_data
                                                            [i].note,
                                                            total++);
                                        }
                                        if (what & 2) {
                                                /* printf("sample %02x ", pattern_data[i].sample); */
                                                output_byte(output_s,
                                                            pattern_data
                                                            [i].sample,
                                                            total++);
                                        }
                                        if (what & 4) {
                                                /* printf("volume %02x ", pattern_data[i].volume); */
                                                output_byte(output_s,
                                                            pattern_data
                                                            [i].volume,
                                                            total++);
                                        }
                                        if (what & 8) {
                                                /* printf("effect %02x param %02x", pattern_data[i].effect, pattern_data[i].param); */
                                                output_byte(output_s,
                                                            pattern_data
                                                            [i].effect,
                                                            total++);
                                                output_byte(output_s,
                                                            pattern_data
                                                            [i].param,
                                                            total++);
                                        }
                                        /* printf("\n"); */
                                }
                        }
                        if (last_channel != (channels - 1)) {
                                /* premature end byte */
                                output_byte(output_s, 0, total++);
                                /* total++; */
                        }
                }
        }
        if (globals.verbose)
                printf("Packed %d unpacked %d\n", total, length);
#if 0
        int i;
        for (i = 0; i < length; i++) {
                /* output to the s file */
                if ((i % 4) == 0) {
                        fprintf(output_s, "\n.byte ");
                } else {
                        fprintf(output_s, ",");
                }
                fprintf(output_s, "%d,", pattern_data[i].note);
                fprintf(output_s, "%d,", pattern_data[i].sample);
                fprintf(output_s, "%d,", pattern_data[i].volume);
                fprintf(output_s, "%d,", pattern_data[i].effect);
                fprintf(output_s, "%d", pattern_data[i].param);
        }
#endif
        fprintf(output_s, "\n/* end roXm%dPattern%d */\n\n", globals.file_index, index);
        fflush(output_s);
}

void output_sample(int index,
                   char *sample_data, int length, FILE *output_s)
{
        fprintf(output_s, ".align 2\n");
        if (globals.use_underscore)
                fprintf(output_s, ".global _roXm%dSample%d\n", globals.file_index, index);
        else
                fprintf(output_s, ".global roXm%dSample%d\n", globals.file_index, index);
        if (globals.use_underscore)
                fprintf(output_s, "_roXm%dSample%d:", globals.file_index, index);
        else
                fprintf(output_s, "roXm%dSample%d:", globals.file_index, index);
        int i;
        for (i = 0; i < length; i++) {
                /* output to the s file */
                if ((i % 16) == 0) {
                        fprintf(output_s, "\n.byte ");
                } else {
                        fprintf(output_s, " , ");
                }
                if (globals.use_unsigned) {
                        /* understand that the data is unsigned */
                        /* 127 -> 255 */
                        /* 0 -> 128 */
                        /* -128 -> 0 */
                        int o = ((int) sample_data[i]) + 128;
                        fprintf(output_s, "%d", o);
                } else {
                        fprintf(output_s, "%d", sample_data[i]);
                }
        }
        fprintf(output_s, "\n/* end roXm%dSample%d */\n\n",
                globals.file_index, index);
        fflush(output_s);
}

void output_pattern_headers(pattern_header_t *pattern_header, int length,
                            FILE *output_c)
{
        /* output the pattern header table to the c file */
        /*  */
        /* need to add the external symbols... */
        int i;
        for (i = 0; i < length; i++) {
                fprintf(output_c, "extern const uint8_t roXm%dPattern%d[];\n",
                        globals.file_index, pattern_header[i].index);
        }
        fprintf(output_c, "\n\n");
        fprintf(output_c, "/* rows,     data */\n");
        fprintf(output_c, "const pattern_t roXm%dPatterns[%d] = {\n",
                globals.file_index, length);
        for (i = 0; i < length; i++) {
                fprintf(output_c, "  {%4d, roXm%dPattern%d},\n",
                        pattern_header[i].rows, globals.file_index,
                        pattern_header[i].index);
        }
        fprintf(output_c, "};\n\n");
        fflush(output_c);
}

void output_start_sample_table(FILE *output_c)
{
        /* first, a hack - I need to add the extern declarations before referencing */
        /* the samples, but can't know how many samples there are until the whole of */
        /* the instrument section has been read as each instrument has a variable */
        /* number of samples.  In order to save reading the instrument section twice, */
        /* once to get the sample count then again to create the sample data and the */
        /* sample header table, I just declare the maximum (128) sample names  - */
        /* better to have unused extern declarations than to reference names that */
        /* aren't declared */
        fprintf(output_c,
                "/* Note that not all of these declared samples exist,\n   it's just easier to do it this way*/\n");
        int i;
        for (i = 0; i < 128; i++)
        {
                if (globals.use_unsigned)
                        fprintf(output_c, "extern const uint8_t roXm%dSample%d[];\n", globals.file_index, i);
                else
                        fprintf(output_c, "extern const int8_t roXm%dSample%d[];\n", globals.file_index, i);
        };
        fprintf(output_c, "\n\n/*  size, loop, l-length, reltve, fine, vol, pan, data, fadeout */\n");
        fprintf(output_c, "const sample_t roXm%dSamples[] ={\n", globals.file_index);
        fflush(output_c);
}

void output_sample_table(int findex, int index, sample_t *s, FILE *output_c)
{
        int i;
        if (s)
        {
                fprintf(output_c,
                        "  { %5d, %5d, %5d, %4d, %4d, %3d, %3d, roXm%dSample%d, %5d",
                        s->length, s->loop_start, s->loop_length, s->relative,
                        s->finetune, s->vol, s->pan, findex, index, s->volume_fade);
                if (globals.inc_envelope)
                {
                        fprintf(output_c, ", {");
                        for (i=0; i<23; i++)
                                fprintf(output_c, " %5d,", s->venv_pts[i]);
                        fprintf(output_c,
                                " %5d }, %3d, %3d, %3d, %3d, %3d },\n",
                                s->venv_pts[23], s->num_venv_pts, s->venv_sust_pt, s->venv_loop_start, s->venv_loop_end, s->venv_type);
                }
                else
                {
                        fprintf(output_c, " },\n");
                }
                fflush(output_c);
        }
        else
        {
                fprintf(output_c, "  {     0,     0,     0,    0,    0,   0,   0,            0,     0");
                if (globals.inc_envelope)
                {
                        fprintf(output_c, ", { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,");
                        fprintf(output_c, "     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },");
                        fprintf(output_c, "   0,   0,   0,   0,   0 },\n");
                }
                else
                {
                        fprintf(output_c, " },\n");
                }
                fflush(output_c);
        }
}

void output_end_sample_table(FILE *output_c)
{
        fprintf(output_c, "\n}; /* end roXm%dSamples */\n\n\n",
                globals.file_index);
        fflush(output_c);
}

void output_order_table(int length, unsigned char *order, FILE *output_c)
{
        fprintf(output_c, "\n\nconst uint8_t roXm%dOrder[%d] = {",
                globals.file_index, length);
        int i;
        for (i = 0; i < length; i++) {
                if ((i % 8) == 0) {
                        fprintf(output_c, "\n  ");
                        fflush(output_c);
                }
                fprintf(output_c, "0x%02x, ", order[i]);
        }
        fprintf(output_c, "\n};\n\n");
        fflush(output_c);
}

void output_mod_header(mod_t *mod, FILE *output_c)
{
        fprintf(output_c, "const mod_t roXm%dSong = {\n",
                globals.file_index);
        fprintf(output_c, "   roXm%dPatterns,\n", globals.file_index);
        fprintf(output_c, "   roXm%dSamples,\n", globals.file_index);
        fprintf(output_c, "   roXm%dOrder,\n", globals.file_index);
        fprintf(output_c, "   %d, /* order length */\n", mod->song_length);
        fprintf(output_c, "   %d, /* restart position */\n", mod->restart);
        fprintf(output_c, "   %d, /* default speed */\n", mod->speed);
        fprintf(output_c, "   %d, /* default tempo (BPM) */\n", mod->tempo);
        fprintf(output_c, "   %d, /* channels */\n", mod->channels);
        fprintf(output_c, "};\n");
        fflush(output_c);
}

void output_module_table(FILE *output_c)
{
        fprintf(output_c, "\nconst mod_t * const song_table[%d] = {\n",
                globals.file_index);
        int i;
        for (i = 0; i < globals.file_index; i++) {
                fprintf(output_c, "  &roXm%dSong,\n", i);
        }
        fprintf(output_c, "};\n");
        fflush(output_c);
}

void output_modules_header(FILE *output_h)
{
        int i;
        fprintf(output_h, "#ifndef modules_h_seen\n");
        fprintf(output_h, "#define modules_h_seen\n");
        output_init_c(output_h);
        for (i = 0; i < globals.file_index; i++) {
                if (globals.song_titles[i] == 0) {
                        fprintf(output_h, "#define  SONG_%d  %d\n", i, i);
                } else {
                        fprintf(output_h, "#define  SONG_%s  %d\n",
                                globals.song_titles[i], i);
                }
        }
        fprintf(output_h, "\nextern const mod_t * const song_table[%d];\n",
                globals.file_index);
        fprintf(output_h, "#endif /* modules_h_seen */\n");
        fflush(output_h);
}
