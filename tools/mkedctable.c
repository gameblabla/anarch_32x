// Make Equal Distance Crossfade Table - to build, enter in terminal:
//  gcc -o mkedctable mkedctable.c -lm

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(void)
{
    int pan;
    float amp;

    FILE *fh = fopen("../edc_table.i", "w");
    fprintf(fh, "/* Equal Distance Crossfade lookup tables */\n\n");
    fprintf(fh, "edc_left:\n");
    for (pan=0; pan<129; pan++)
    {
        // left amp = cos(Pi/2 * pan/256)
        amp = cosf(3.1415927 / 2.0 * (float)pan / 256.0);
        fprintf(fh, "        .byte%6d /* %3d (%f) */\n", (int)(amp * 255.0), pan, amp);
    }
    // The pan is split because the two halves are not equal - 0 to 128 is left to center,
    // but center to right is 128 to 255. The difference in divisors makes up for that.
    for (pan=129; pan<256; pan++)
    {
        // left amp = cos(Pi/2 * pan/255)
        amp = cosf(3.1415927 / 2.0 * (float)pan / 255.0);
        fprintf(fh, "        .byte%6d /* %3d (%f) */\n", (int)(amp * 255.0), pan, amp);
    }
    fprintf(fh, "\nedc_right:\n");
    for (pan=0; pan<129; pan++)
    {
        // right amp = sin(Pi/2 * pan/256)
        amp = sinf(3.1415927 / 2.0 * (float)pan / 256.0);
        fprintf(fh, "        .byte%6d /* %3d (%f) */\n", (int)(amp * 255.0), pan, amp);
    }
    // The pan is split because the two halves are not equal - 0 to 128 is left to center,
    // but center to right is 128 to 255. The difference in divisors makes up for that.
    for (pan=129; pan<256; pan++)
    {
        // right amp = sin(Pi/2 * pan/255)
        amp = sinf(3.1415927 / 2.0 * (float)pan / 255.0);
        fprintf(fh, "        .byte%6d /* %3d (%f) */\n", (int)(amp * 255.0), pan, amp);
    }
    fclose(fh);
}
