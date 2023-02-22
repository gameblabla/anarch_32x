#include "output.h"
#include <malloc.h>
#include <ctype.h>
#include <string.h>

typedef struct __attribute__((packed)) {
        char id_text[17];
        char name[20];
        char _1ah;
        char tracker[20];
        unsigned short version;
        unsigned long header_size;
        unsigned short song_length;
        unsigned short restart;
        unsigned short channels;
        unsigned short patterns;
        unsigned short instruments;
        unsigned short flags;
        unsigned short tempo;
        unsigned short BPM;
} xm_header_t;

typedef struct __attribute__((packed)) {
        unsigned long header_length;
        unsigned char type;
        unsigned short row;
        unsigned short packed_size;
} xm_pattern_header_t;

typedef struct __attribute__((packed)) {
        unsigned long header_length;
        char name[22];
        char type;
        unsigned short samples;
} xm_instrument_header_1_t;

typedef struct __attribute__((packed)) {
        unsigned long sample_header_size;
        char sample_number[96];
        unsigned short envelope_volume[24];
        unsigned short envelope_panning[24];
        unsigned char nvp;
        unsigned char npp;
        unsigned char vsp;
        unsigned char vls;
        unsigned char vle;
        unsigned char psp;
        unsigned char pls;
        unsigned char ple;
        unsigned char v_t;
        unsigned char p_t;

        unsigned char vbt;
        unsigned char vbsw;
        unsigned char vbd;
        unsigned char vbr;
        unsigned short volume_fade;
        unsigned char res[11];

} xm_instrument_header_2_t;

typedef struct __attribute__((packed)) {
        unsigned long sample_length;
        unsigned long loop_start;
        unsigned long loop_length;
        unsigned char volume;
        signed char finetune;
        unsigned char type;
        unsigned char panning;
        signed char relative_note;
        unsigned char x;
        char name[22];

} xm_sample_header_t;

//#define ALL_SAMPLES /* will include all samples, including repeats */
#ifndef ALL_SAMPLES
static sample_t samps[128][128];
#endif

void read_xm(FILE *fp, FILE *output_s, FILE *output_c)
{
        int i;
        if (globals.verbose) {
                printf("read xm\n");
        }
        xm_header_t header;
        fseek(fp, 0, SEEK_SET);
        fread(&header, sizeof(xm_header_t), 1, fp);
        if (globals.verbose) {
                header.id_text[16] = 0;
                printf("xm id  : %s\n", header.id_text);
                printf("xm name: %s\n", header.name);
                printf("xm trkr: %s\n", header.tracker);
                printf("xm vers: %04x\n", header.version);
                printf("xm head: %lu\n", header.header_size);
                printf("xm song: %d\n", header.song_length);
                printf("xm rest: %d\n", header.restart);
                printf("xm chnl: %d\n", header.channels);
                printf("xm patt: %d\n", header.patterns);
                printf("xm inst: %d\n", header.instruments);
                printf("xm flag: %x (%s)\n", header.flags,
                       (header.flags & 1) ? "Linear" : "Amiga");
                printf("xm tmpo: %d\n", header.tempo);
                printf("xm BPM : %d\n", header.BPM);
        }
        int title_len = strlen(header.name);
        if (title_len > 0) {
                globals.song_titles[globals.file_index] =
                        (char *) malloc(sizeof(char) * (title_len * 4));
                for (i = 0; i < title_len; i++) {
                        if (isascii(header.name[i])
                            && isalnum(header.name[i])) {
                                globals.song_titles[globals.file_index][i] = isalpha(header.name[i]) ? header.name[i] & ~0x20 : header.name[i];
                        } else {
                                globals.song_titles[globals.file_index][i] = '_';
                        }
                }
                globals.song_titles[globals.file_index][i] = 0;
        } else {
                globals.song_titles[globals.file_index] = 0;
        }
        /* the pattern order table follows */
        unsigned char orders[256];
        fread(orders, 1, 256, fp);
        if (globals.verbose) {
                printf("\nOrder: ");
                for (i = 0; i < header.song_length; i++) {
                        printf("%#02x ", orders[i]);
                }
                printf("\n\n");
        }
        output_order_table(header.song_length, orders, output_c);
        /* the patterns follow: */
        /* header of pattern, data, header, data, etc for header.patterns */
        xm_pattern_header_t *patheader = (xm_pattern_header_t *)
                                         malloc(sizeof(xm_pattern_header_t) * header.patterns);
        for (i = 0; i < header.patterns; i++) {
                fread(&patheader[i], sizeof(xm_pattern_header_t), 1, fp);
                if (0 && globals.verbose) {
                        printf("pat hedl: %lu\n",
                               patheader[i].header_length);
                        printf("pat type: %d\n", patheader[i].type);
                        printf("pat rown: %d\n", patheader[i].row);
                        printf("pat pksz: %d\n\n",
                               patheader[i].packed_size);
                }
                if (patheader[i].packed_size == 0) {
                        /* empty pattern */
                        printf("empty pattern... creating empty pattern\n");
                        pattern_t *pattern_data =
                                (pattern_t *) malloc(64 *
                                                     sizeof(pattern_t) *
                                                     header.channels);
                        memset(pattern_data, 0,
                               (64 * sizeof(pattern_t) *
                                header.channels));
                        ASSERT(pattern_data);
                        /* output the pattern data to the S file */
                        output_patterns(i, pattern_data, 64,
                                        header.channels, output_s);
                        free(pattern_data);
                } else {
                        /* output_pattern_header(, output_c); */
                        /* unpack pattern */
                        /* I guess that there are channels*bytes_per_note*rows_per_pattern in a pattern? */
                        /* unpack */
                        pattern_t *pattern_data =
                                (pattern_t *) malloc(patheader[i].row *
                                                     sizeof(pattern_t) *
                                                     header.channels);
                        memset(pattern_data, 0,
                               (patheader[i].row * sizeof(pattern_t) *
                                header.channels));
                        /* printf("Unpacked pattern size = %d\n", (patheader[i].row*sizeof(pattern_t)*header.channels)); */
                        ASSERT(pattern_data);
                        int l;
                        int current_channel = 0;
                        /* int current_row = 0; */
                        for (l = 0; l < patheader[i].packed_size;) {
                                unsigned char what = fgetc(fp);
                                l++;
                                if ((what & 0x80) == 0) {
                                        /* top bit not set - a full "pattern set" follows */
                                        pattern_data[current_channel].
                                        note = what;
                                        what = fgetc(fp);
                                        l++;
                                        pattern_data[current_channel].
                                        sample = what;
                                        what = fgetc(fp);
                                        l++;
                                        pattern_data[current_channel].
                                        volume = what;
                                        what = fgetc(fp);
                                        l++;
                                        pattern_data[current_channel].
                                        effect = what;
                                        what = fgetc(fp);
                                        l++;
                                        pattern_data[current_channel++].
                                        param = what;
                                } else {
                                        if (what & 1) {
                                                unsigned char note =
                                                        fgetc(fp);
                                                l++;
                                                pattern_data
                                                [current_channel].
                                                note = note;
                                        }
                                        if (what & 2) {
                                                pattern_data
                                                [current_channel].
                                                sample = fgetc(fp);
                                                l++;
                                        }
                                        if (what & 4) {
                                                pattern_data
                                                [current_channel].
                                                volume = fgetc(fp);
                                                l++;
                                        }
                                        if (what & 8) {
                                                pattern_data
                                                [current_channel].
                                                effect = fgetc(fp);
                                                l++;
                                        }
                                        if (what & 16) {
                                                pattern_data
                                                [current_channel].
                                                param = fgetc(fp);
                                                l++;
                                        }
                                        current_channel++;
                                }
//                                if (pattern_data[current_channel - 1].
//                                    volume > 64) {
//                                        /* printf("Volume > 64: %d\n",pattern_data[current_channel-1].volume); */
//                                        /* pattern_data[current_channel-1].volume /= 2; */
//                                        pattern_data[current_channel -
//                                                     1].volume = 64;
//                                }
                        }
                        /* actually, that packing scheme isn't too bad - leave it like that? */
                        /* or use s3m style packing, which could be faster for lots of empty rows? */
                        /* dump the pattern to a file */
                        /*  */
                        if (0 && globals.verbose) {
                                int row = 0;
                                for (row = 0; row < patheader[i].row;
                                     row++) {
                                        int ch = 0;
                                        for (ch = 0; ch < header.channels;
                                             ch++) {
                                                printf("%02x %2d   |",
                                                       pattern_data[row *
                                                                    header.
                                                                    channels
                                                                    +
                                                                    ch].
                                                       note,
                                                       pattern_data[row *
                                                                    header.
                                                                    channels
                                                                    +
                                                                    ch].
                                                       sample);
                                        }
                                        printf("\n");
                                        for (ch = 0; ch < header.channels;
                                             ch++) {
                                                printf("%2d %02x %02x|",
                                                       pattern_data[row *
                                                                    header.
                                                                    channels
                                                                    +
                                                                    ch].
                                                       volume,
                                                       pattern_data[row *
                                                                    header.
                                                                    channels
                                                                    +
                                                                    ch].
                                                       effect,
                                                       pattern_data[row *
                                                                    header.
                                                                    channels
                                                                    +
                                                                    ch].
                                                       param);
                                        }
                                        printf
                                        ("\n-------------- ROW %4d  ---------------------\n",
                                         row);
                                }
                        }
                        /* output the pattern data to the S file */
                        output_patterns(i, pattern_data, patheader[i].row,
                                        header.channels, output_s);
                        free(pattern_data);
                }
        }
        /* output the headers to a file */
        pattern_header_t *gba_pattern_header =
                (pattern_header_t *) malloc(sizeof(pattern_header_t) *
                                            header.patterns);
        for (i = 0; i < header.patterns; i++) {
                gba_pattern_header[i].rows = patheader[i].row;
                gba_pattern_header[i].index = i;
        }
        output_pattern_headers(gba_pattern_header, header.patterns,
                               output_c);
        free(patheader);
        /* after the patterns comes the instruments */
        /* here it gets complex as instruments use variable amounts of samples */
        /* we don't know quite how many until after reading the file */
        /*  use output_start_sample_table, "const sample_t samples[]={" */
        /* then fill in the middle as we go  "{blah blah }", */
        /* then output_end_sample_table   "};" */
        output_start_sample_table(output_c);
        int gba_sample_index = 0;
        for (i = 0; i < header.instruments; i++) {
                xm_instrument_header_1_t inshead;
                int inst_head_pos = ftell(fp);
                fread(&inshead, sizeof(xm_instrument_header_1_t), 1, fp);
                if (globals.verbose) {
                        printf("ins samp: %d\n", inshead.samples);
                        printf("ins hdln: %lu\n", inshead.header_length);
                        printf("ins name: %s\n", inshead.name);
                }
                xm_instrument_header_2_t inshead2;
                if (inshead.samples) {
                        fread(&inshead2, sizeof(xm_instrument_header_2_t),
                              1, fp);
                        if (globals.verbose) {
                                printf("in2 shsz: %lu\n",
                                       inshead2.sample_header_size);
                                printf("in2 nvp : %d\n", inshead2.nvp);
                                printf("in2 npp : %d\n", inshead2.npp);
                                printf("in2 vfo : %d\n", inshead2.volume_fade);
                        }
                }
                fseek(fp, inst_head_pos + inshead.header_length, SEEK_SET);
                if (inshead.samples > 0) {
                        int j;
                        xm_sample_header_t *samphead =
                                (xm_sample_header_t *)
                                malloc(sizeof(xm_sample_header_t) *
                                       inshead.samples);
                        for (j = 0; j < inshead.samples; j++) {
                                fread(&samphead[j],
                                      sizeof(xm_sample_header_t), 1, fp);
                        }
                        /* after the headers, the sample data follows! */
                        for (j = 0; j < inshead.samples; j++) {
                                if (globals.verbose) {
                                        printf("sam name: %s\n",
                                               samphead[j].name);
                                        printf("sam leng: %lu\n",
                                               samphead[j].sample_length);
                                        printf("sam fine: %d\n", samphead[j].finetune); /* <-- finetune adjusts pitch of note up or down */
                                        printf("sam lpln: %lu\n",
                                               samphead[j].loop_length);
                                        printf("sam  pan: %d\n",
                                               samphead[j].panning);
                                        printf("sam reln: %d\n",
                                               samphead[j].relative_note);
                                        printf
                                        ("sam type: %#02x (if &0x10, 16bit)\n",
                                         samphead[j].type);
                                }
                                if (samphead[j].sample_length) {
                                        /* TODO - read the sample data , dump to file etc */
                                        /* if data is 16 bit, need to /256 */
                                        /* data is stored in a bizarre delta format */
                                        char *sample_data =
                                                (char *) malloc(samphead[j].
                                                                sample_length);
                                        if (samphead[j].type & 0x10) {
                                                /* 16-bit sample */
                                                short *sd =
                                                        (short *)
                                                        malloc(samphead[j].
                                                               sample_length);
                                                samphead[j].
                                                sample_length /= 2;
                                                samphead[j].
                                                loop_start /= 2;
                                                samphead[j].
                                                loop_length /= 2;
                                                fread(sd, 2,
                                                      samphead[j].
                                                      sample_length, fp);
                                                int l = 0;
                                                short old = 0;
                                                short nw;
                                                for (l = 0;
                                                     l <
                                                     samphead[j].
                                                     sample_length; l++) {
                                                        nw = sd[l] + old;
                                                        sample_data[l] =
                                                                nw / 256;
                                                        old = nw;
                                                }
                                                free(sd);
                                        } else {
                                                /* 8 bit sample */
                                                fread(sample_data, 1,
                                                      samphead[j].
                                                      sample_length, fp);
                                                int l = 0;
                                                char old = 0;
                                                char nw;
                                                for (l = 0;
                                                     l <
                                                     samphead[j].
                                                     sample_length; l++) {
                                                        nw = sample_data[l]
                                                             + old;
                                                        sample_data[l] =
                                                                nw;
                                                        old = nw;
                                                }
                                                /* dump sample */
                                        }
                                        if ((samphead[j].type & 3) == 0) {
                                                /* no loop - clear loop vars */
                                                samphead[j].loop_start = 0;
                                                samphead[j].loop_length = 0;
                                        }
                                        else if ((samphead[j].type & 3) == 2) {
                                                int ix;
                                                /* unroll ping-pong loop */
                                                char *sd = (char *)malloc(samphead[j].sample_length + samphead[j].loop_length);
                                                memcpy(sd, sample_data, samphead[j].sample_length);
                                                //memcpy(sd + samphead[j].sample_length, sample_data + samphead[j].loop_start, samphead[j].loop_length);
                                                for (ix=0; ix<samphead[j].loop_length; ix++)
                                                        sd[samphead[j].loop_start + samphead[j].loop_length + ix] = sample_data[samphead[j].loop_start + samphead[j].loop_length - ix - 1];
                                                free(sample_data);
                                                sample_data = sd;
                                                samphead[j].sample_length += samphead[j].loop_length;
                                                samphead[j].loop_length += samphead[j].loop_length;
                                        }
                                        if (j == 0) {
                                                int ii, jj, kk;
                                                unsigned int cc, a = 1, b = 0;
                                                /* compute Adler32 for sample data */
                                                for (ii=0; ii<samphead[j].sample_length; ii++)
                                                {
                                                        a = (a + sample_data[ii]) % 65521;
                                                        b = (b + a) % 65521;
                                                }
                                                cc = (b << 16) | a;
                                                if (globals.file_index == 0 && gba_sample_index == 0)
                                                {
                                                        output_sample
                                                        (gba_sample_index,
                                                         sample_data,
                                                         samphead[j].
                                                         sample_length,
                                                         output_s);
                                                        samps[globals.file_index][gba_sample_index].length = samphead[j].sample_length;
                                                        samps[globals.file_index][gba_sample_index].loop_start = samphead[j].loop_start;
                                                        samps[globals.file_index][gba_sample_index].loop_length = samphead[j].loop_length;
                                                        samps[globals.file_index][gba_sample_index].relative = samphead[j].relative_note;
                                                        samps[globals.file_index][gba_sample_index].finetune = samphead[j].finetune;
                                                        samps[globals.file_index][gba_sample_index].vol = samphead[j].volume;
                                                        samps[globals.file_index][gba_sample_index].pan = samphead[j].panning;
                                                        samps[globals.file_index][gba_sample_index].data = 0;
                                                        samps[globals.file_index][gba_sample_index].volume_fade = inshead2.volume_fade;
                                                        memcpy(&samps[globals.file_index][gba_sample_index].venv_pts, &inshead2.envelope_volume, 48);
                                                        samps[globals.file_index][gba_sample_index].num_venv_pts = inshead2.nvp;
                                                        samps[globals.file_index][gba_sample_index].venv_sust_pt = inshead2.vsp;
                                                        samps[globals.file_index][gba_sample_index].venv_loop_start = inshead2.vls;
                                                        samps[globals.file_index][gba_sample_index].venv_loop_end = inshead2.vle;
                                                        samps[globals.file_index][gba_sample_index].venv_type = inshead2.v_t;
                                                        samps[globals.file_index][gba_sample_index].checksum = cc;
                                                        output_sample_table
                                                        (globals.file_index, gba_sample_index, &samps[globals.file_index][gba_sample_index],
                                                         output_c);
                                                }
                                                else
                                                {
                                                        kk = 0;
                                                        /* check for pre-existing sample */
                                                        for (ii=0; ii<=globals.file_index; ii++)
                                                        {
                                                                for (jj=0; jj<128; jj++)
                                                                        if (samps[ii][jj].length == samphead[j].sample_length &&
                                                                            samps[ii][jj].loop_start == samphead[j].loop_start &&
                                                                            samps[ii][jj].loop_length == samphead[j].loop_length &&
                                                                            samps[ii][jj].checksum == cc)
                                                                        {
                                                                                kk = 1; /* sample is a copy */
                                                                                break;
                                                                        }
                                                                if (kk)
                                                                        break;
                                                        }
                                                        if (!kk)
                                                                output_sample
                                                                (gba_sample_index,
                                                                 sample_data,
                                                                 samphead[j].
                                                                 sample_length,
                                                                 output_s);
                                                        samps[globals.file_index][gba_sample_index].length = samphead[j].sample_length;
                                                        samps[globals.file_index][gba_sample_index].loop_start = samphead[j].loop_start;
                                                        samps[globals.file_index][gba_sample_index].loop_length = samphead[j].loop_length;
                                                        samps[globals.file_index][gba_sample_index].relative = samphead[j].relative_note;
                                                        samps[globals.file_index][gba_sample_index].finetune = samphead[j].finetune;
                                                        samps[globals.file_index][gba_sample_index].vol = samphead[j].volume;
                                                        samps[globals.file_index][gba_sample_index].pan = samphead[j].panning;
                                                        samps[globals.file_index][gba_sample_index].data = 0;
                                                        samps[globals.file_index][gba_sample_index].volume_fade = inshead2.volume_fade;
                                                        memcpy(&samps[globals.file_index][gba_sample_index].venv_pts, &inshead2.envelope_volume, 48);
                                                        samps[globals.file_index][gba_sample_index].num_venv_pts = inshead2.nvp;
                                                        samps[globals.file_index][gba_sample_index].venv_sust_pt = inshead2.vsp;
                                                        samps[globals.file_index][gba_sample_index].venv_loop_start = inshead2.vls;
                                                        samps[globals.file_index][gba_sample_index].venv_loop_end = inshead2.vle;
                                                        samps[globals.file_index][gba_sample_index].venv_type = inshead2.v_t;
                                                        samps[globals.file_index][gba_sample_index].checksum = cc;
                                                        output_sample_table
                                                        (kk ? ii : globals.file_index, kk ? jj : gba_sample_index, &samps[globals.file_index][gba_sample_index],
                                                         output_c);
                                                }
                                                gba_sample_index++;
                                        }
                                        free(sample_data);
                                }
                        }
                        free(samphead);
                }
                else
                {
                        // empty instrument - no samples!
                        output_sample_table(globals.file_index, gba_sample_index, (sample_t *)NULL, output_c);
                        gba_sample_index++;
                }
        }
        output_end_sample_table(output_c);
        mod_t mod_header = {
                0, 0, 0,        /* data */
                header.song_length,
                header.restart,
                header.tempo,
                header.BPM,
                header.channels
        };
        output_mod_header(&mod_header, output_c);
}
