#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

#define SAMPLE_RATE    22050
#define SAMPLE_MIN         2
#define SAMPLE_CENTER    517
#define SAMPLE_MAX      1032
#define MAX_NUM_SAMPLES 1024

#define NUM_SFX_MIXERS 4

extern uint16_t num_samples;
extern int16_t snd_buffer[];

extern void fill_buffer(int16_t *buffer);
extern uint8_t start_sfx(int8_t *data, uint16_t srate, uint32_t loop_length, uint32_t length, uint8_t volume, uint8_t pan);
extern void update_sfx(uint8_t mix, int32_t srate, int32_t volume, int32_t pan);
extern void stop_sfx(uint8_t mix);
extern uint8_t check_sfx(uint8_t mix);

#endif
