/**
  @file main_32x.c

  This is an 32X implementation of the game front end. It can be used to
  compile a native executable or a transpiled JS browser version with
  emscripten.

  by Miloslav Ciz (drummyfish), 2019
  32X port by Gameblabla

  Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
  plus a waiver of all other intellectual property. The goal of this work is to
  be and remain completely in the public domain forever, available for any use
  whatsoever.
*/

// #define SFG_START_LEVEL 1
// #define SFG_QUICK_WIN 1
// #define SFG_IMMORTAL 1
// #define SFG_ALL_LEVELS 1
// #define SFG_UNLOCK_DOOR 1
// #define SFG_REVEAL_MAP 1
// #define SFG_INFINITE_AMMO 1
// #define SFG_TIME_MULTIPLIER 512
// #define SFG_CPU_LOAD(percent) printf("CPU load: %d%\n",percent);
// #define GAME_LQ

// lower quality
#define SFG_FPS 20
#define SFG_SCREEN_RESOLUTION_X 320
#define SFG_SCREEN_RESOLUTION_Y 192
#define SFG_RAYCASTING_SUBSAMPLE 3
#define SFG_RESOLUTION_SCALEDOWN 2
#define SFG_DIMINISH_SPRITES 0
#define SFG_DITHERED_SHADOW 0
#define SFG_BACKGROUND_BLUR 0
#define SFG_RAYCASTING_MAX_STEPS 15
#define SFG_RAYCASTING_MAX_HITS 5
#define SFG_RAYCASTING_VISIBILITY_MAX_HITS 6
#define SFG_CAN_EXIT 0
#define SFG_DRAW_LEVEL_BACKGROUND 1

/*
  SDL is easier to play thanks to nice controls so make the player take full
  damage to make it a bit harder.
*/
#define SFG_PLAYER_DAMAGE_MULTIPLIER 1024


#include <stdio.h>
#include <stdlib.h>
#include <xmp.h>
#include <math.h>
#include "types.h"
#include "32x.h"
#include "hw_32x.h"
#include "32x_images.h"
#include "game_constants.h"
#include "shared_objects.h"
#include "aplib_decrunch.h"

#include "game.h"
#include "sound.h"
#include "sounds.h"
#include "files.h"

uint32_t ticks = 0;

unsigned short curr_buttons;
extern char pal[], pal_end[];
unsigned short curr_buttons;

vu8 *framebuffer = (vu8* ) &MARS_FRAMEBUFFER + 0x200 + 320*16;


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
void SFG_clearScreen(uint8_t color)
{
	Hw32xScreenClear();
}*/

// now implement the Anarch API functions (SFG_*)

void SFG_setPixel(uint16_t x, uint16_t y, uint8_t colorIndex)
{
#ifdef SIXTEEN
	framebuffer16[y * 320 + x] = paletteRGB565[colorIndex];
	
	//*((unsigned short*)framebuffer16 + x + y * 80) = paletteRGB565[colorIndex];
	//framebuffer16[y * SFG_SCREEN_RESOLUTION_X + x] = paletteRGB565[colorIndex];
#else
	framebuffer[y * SFG_SCREEN_RESOLUTION_X + x] = colorIndex;
#endif
}

uint32_t SFG_getTimeMs()
{
  return ticks * 100;
}

void SFG_save(uint8_t data[SFG_SAVE_SIZE])
{
  /*FILE *f = fopen(SFG_SAVE_FILE_PATH,"wb");

  puts("SDL: opening and writing save file");

  if (f == NULL)
  {
    puts("SDL: could not open the file!");
    return;
  }

  fwrite(data,1,SFG_SAVE_SIZE,f);

  fclose(f);*/
}

uint8_t SFG_load(uint8_t data[SFG_SAVE_SIZE])
{/*
#ifndef __EMSCRIPTEN__
  FILE *f = fopen(SFG_SAVE_FILE_PATH,"rb");

  puts("SDL: opening and reading save file");

  if (f == NULL)
  {
    puts("SDL: no save file to open");
  }
  else
  {
    fread(data,1,SFG_SAVE_SIZE,f);
    fclose(f);
  }

  return 1;
#else
  // no saving for web version
  return 0;
#endif*/
}

void SFG_sleepMs(uint16_t timeMs)
{
}



void SFG_getMouseOffset(int16_t *x, int16_t *y)
{
/*
#ifndef __EMSCRIPTEN__

#if !defined(__WIIU__)
  if (mouseMoved)
  {
    int mX, mY;

    SDL_GetMouseState(&mX,&mY);

    *x = mX - SFG_SCREEN_RESOLUTION_X / 2;
    *y = mY - SFG_SCREEN_RESOLUTION_Y / 2;

    SDL_WarpMouseInWindow(window,
      SFG_SCREEN_RESOLUTION_X / 2, SFG_SCREEN_RESOLUTION_Y / 2);
  }
#endif
  if (sdlController != NULL)
  {
    *x +=
      (SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_RIGHTX) + 
      SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_LEFTX)) /
      SDL_ANALOG_DIVIDER;

    *y +=
      (SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_RIGHTY) + 
      SDL_GameControllerGetAxis(sdlController,SDL_CONTROLLER_AXIS_LEFTY)) /
      SDL_ANALOG_DIVIDER;
  }
#endif
*/
}

void SFG_processEvent(uint8_t event, uint8_t data)
{
}

int8_t SFG_keyPressed(uint8_t key)
{
  #define b(x) (curr_buttons & SEGA_CTRL_ ## x )

  switch (key)
  {
    case SFG_KEY_UP: return b(UP); break;
    case SFG_KEY_RIGHT: 
      return b(RIGHT); break;
    case SFG_KEY_DOWN: 
      return b(DOWN); break;
    case SFG_KEY_LEFT: return b(LEFT); break;
    case SFG_KEY_A: return b(A); break;
    case SFG_KEY_B: return b(B); break;
    case SFG_KEY_C: return b(C); break;
    case SFG_KEY_JUMP: return b(X); break;
    case SFG_KEY_STRAFE_LEFT: return b(Y); break;
    case SFG_KEY_STRAFE_RIGHT: return b(Z); break;
    //case SFG_KEY_MAP: return b(MINUS); break;
    case SFG_KEY_CYCLE_WEAPON: return b(MODE); break;
    case SFG_KEY_MENU: return b(START); break;
   /* case SFG_KEY_NEXT_WEAPON:
      if (b(ZR))
        return 1;
      else
      return 0;
      break;*/

    default: return 0; break;
  }

  #undef b
}
  
int running = 1;
int curr_mod = 0;
int musicpaused = 0;
unsigned char snd1_mix;

int8_t mvolume = XMP_MAX_VOLUME;
song_t __attribute__((aligned(16))) *song = (song_t *)NULL;

// helper function for libxmp
void *xmp_malloc(int32_t size)
{
    // make sure cache line size aligned
    return memalign(16, size);
}

static void new_song(int index)
{
    // lock the mixer while killing the old music
    LockMixer(MIXER_LOCK_MSH2);
    if (song)
    {
        xmp_stop_song(song);
        free(song);
        song = (song_t *)NULL;
    }
    UnlockMixer();

    Hw32xDelay(2); // allow the mixer to see the music is dead

    // lock the mixer while starting the new music
    LockMixer(MIXER_LOCK_MSH2);
    song = xmp_start_song(index, -1);
    
    curr_mod = index;

    UnlockMixer();
}



void mainLoopIteration()
{
	if (!SFG_mainLoopBody())
		running = 0;

	handle_input();
	Hw32xScreenFlip(1);
	ticks++;
	
	CacheClearLine(song); // just clear shared state variables

	if (!musicpaused && !xmp_is_playing(song))
	{
		xmp_pause(song, 0);
		new_song(curr_mod);
	}
}

uint8_t musicOn = 0;
// ^ this has to be init to 0 (not 1), else a few samples get played at start

void SFG_setMusic(uint8_t value)
{
  switch (value)
  {
    case SFG_MUSIC_TURN_ON: musicpaused = 1; xmp_pause(song, 0);  break;
    case SFG_MUSIC_TURN_OFF: musicpaused = 0; xmp_pause(song, XMP_PAUSED); break;
    case SFG_MUSIC_NEXT:
		curr_mod++;
		if (curr_mod > 1) curr_mod = 0;
		new_song(curr_mod);
		SFG_nextMusicTrack();
	break;
    default: break;
  }
}

void SFG_playSound(uint8_t soundIndex, uint8_t volume)
{
  switch (soundIndex)
  {
    case 2: 
      snd1_mix = start_sfx((char *)filePtr[2], 22050, 0, fileSize[2], volume, 128); // Monster death
      break;

    case 5:
      snd1_mix = start_sfx((char *)filePtr[4], 22050, 0, fileSize[4], volume, 128); // Hurt
      break;

    default:
      snd1_mix = start_sfx((char *)filePtr[0], 22050, 0, fileSize[0], volume, 128); //Click
      break;
  }	
}

void handleSignal(int signal)
{
  running = 0;
}


int main(int argc, char *argv[])
{
	Hw32xInit(MARS_VDP_MODE_256);
	loadPalette(pal , pal_end, 0);
	Hw32xScreenClear();
	
    while (MARS_SYS_COMM6 != MIXER_UNLOCKED) ; // wait for sound subsystem to init
	new_song(0); // start module 0
    
	SFG_init();
	running = 1;

	while (running)
		mainLoopIteration();


  return 0;
}
