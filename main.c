/*
* Copyright 2014 ammianus
*/
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "32x.h"
#include "hw_32x.h"
#include "32x_images.h"
#include "game_constants.h"
#include "graphics.h"
#include "shared_objects.h"
#include "aplib_decrunch.h"

extern const char title;
extern vu16 ex;
extern char pal[], pal_end[];
unsigned short curr_buttons;

static void Flip_screen()
{
    MARS_VDP_FBCTL = currentFB ^ 1;
	while ((MARS_VDP_FBCTL & MARS_VDP_FS) == currentFB) ;
	currentFB ^= 1;
}

/*
* Check the current SEGA Controllers for inputs, update player, direction
* , speed, and action accordingly.
*/
void handle_input()
{
	curr_buttons = MARS_SYS_COMM8;
	if ((curr_buttons & SEGA_CTRL_TYPE) == SEGA_CTRL_NONE)
	{
		curr_buttons = MARS_SYS_COMM10; // if no pad 1, try using pad 2
	}
}


/*
* Starts application
*/
int main(void)
{
	short x = 0, y = 0;
	int i;

	// Wait for the SH2 to gain access to the VDP
	Hw32xInit(MARS_VDP_MODE_256, 0);
	loadPalette(pal, pal_end, 0);
	x = 0;
	y = 0;

	//game loop
	while (1) 
	{
		Flip_screen();
		drawRawBackground(&title);
		drawSprite(&ex, x, y, 96, 96);
		
		if (curr_buttons & SEGA_CTRL_UP)
			y-=1;
		else if (curr_buttons & SEGA_CTRL_DOWN)
			y+=1;
			
		if (curr_buttons & SEGA_CTRL_LEFT) 
			x-=1;
		else if (curr_buttons & SEGA_CTRL_RIGHT)
			x+=1;

		handle_input();
		Update_Video();
	}
	
    HwMdClearScreen ();
    return 0;
} // end of main function

