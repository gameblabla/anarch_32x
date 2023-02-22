/*
* 32X functions for drawing images
* 
* author: ammianus
* 3/3/2012
*
*/
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "32x.h"
#include "aplib_decrunch.h"

#define PIXEL_WRITE_BUFFER_SIZE_B 8
#define FRAMEBUFFER_ROW_EXTENSION 16
#define IS_MIRRORED 1
#define IS_TRANSPARENT 1

#define SCREEN_WIDTH 320

static const int TRANSPARENT_PIXEL_COLOR = 0;
static const vu8 pixelWords[PIXEL_WRITE_BUFFER_SIZE_B] = {0,0,0,0,0,0,0,0};

void Update_Video()
{
	unsigned short i;
	const uint16 lineTableEnd = 0x100;
	uint16 lineOffs;
	vu16 *frameBuffer16 = &MARS_FRAMEBUFFER;
	lineOffs = lineTableEnd;
	for (i = 0; i < 224; i++)
	{
		frameBuffer16[i] = lineOffs;
		lineOffs += 160;
	}
}

/*
* Loads all colors from region in ROM defined by paletteStart to paletteEnd
* into the CRAM
* @param paletteStart - pointer to starting position of palette data
* @param paletteEnd - pointer to end position of palette data
* @param paletteOffset - offset in the CRAM to start writing the palette data to. Normally is 0.
*/
void loadPalette(const vu16 *paletteStart, const vu16 *paletteEnd, const int paletteOffset)
{
	int numColors;
	int i;
	vu16 *cram16 = &MARS_CRAM;
	vu16 *pal16 = (vu16*) paletteStart;
	
	numColors = ((paletteEnd)-(paletteStart));
	for (i = paletteOffset; i < numColors+paletteOffset; i++)
	{
		cram16[i] = pal16[i-paletteOffset] & 0x7FFF;
	}
}
