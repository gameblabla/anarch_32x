// Make Linear Frequency Table - to build, enter in terminal:
//  gcc -o mklfrtable mklfrtable.c -lm

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SAMPLE_RATE 22050.0

int main(void)
{
    int period;
    float fr;
    FILE *fh = fopen("tables.h", "w");
    fprintf(fh, "// Linear Frequencies generated for sample rate of %d Hz\n", (int)SAMPLE_RATE);
    fprintf(fh, "const uint32_t lfr_table[8192] = {\n");
    for (period=0; period<8192; period++)
    {
        // linear frequency = 8363*2^(6*12*64 - period)/(12*64))
        fr = 8363.0 * powf(2.0, ((float)(6*12*64) - (float)period)/((float)(12*64)));
        // sample increment = int(freq*16384/(sample_rate*2))
        fprintf(fh, "\t%d, // %d (%f)\n", (int)((fr * 16384.0 / (SAMPLE_RATE * 2.0)) + 0.5), period, fr);
    }
    fprintf(fh, "};\n");
    fclose(fh);
}
