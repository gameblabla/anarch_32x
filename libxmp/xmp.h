#ifndef XMP_H
#define XMP_H

#include <stdint.h>

typedef struct
{
    uint32_t length;
    uint32_t loop_start;
    uint32_t loop_length;
    int8_t   relative;
    int8_t   finetune;
    uint8_t  vol;
    uint8_t  pan;
    const int8_t *data;         // pointer to data
    uint16_t fadeout;
#ifdef USE_VOL_ENVELOPE
    uint16_t venv_pts[24];
    uint8_t  num_venv_pts;
    uint8_t  venv_sust_pt;
    uint8_t  venv_loop_start;
    uint8_t  venv_loop_end;
    uint8_t  venv_type;
#endif
} sample_t;

typedef struct
{
    uint32_t rows;              // 64 for mod, variable for XM
    const uint8_t *data;        // pattern data
} pattern_t;

typedef struct
{
    const pattern_t *pattern;   // i.e. pattern[order[1]].data[index++]  for note, etc
    const sample_t *sample;
    const uint8_t *order;       // i.e. order[1]
    uint16_t song_length;
    uint16_t restart;
    uint8_t  speed;             // ticks per row (Fxx xx < 0x20)
    uint8_t  tempo;             // BPM           (Fxx xx >= 0x20)
    uint8_t  channels;
} mod_t;

typedef struct
{
    const int8_t *data;         // points to sample data rom, or 0 for channel off
    uint32_t position;          // current position in the data (fixed point 18.14)
    uint32_t increment;         // step size (fixed point 18.14) for pitch
    uint32_t length;            // size of data (fixed point 18.14)
    uint32_t loop_length;       // size of section to loop (fixed point 18.14) or 0 for no loop
    int8_t   volume;            // 0 to 64
    uint8_t  pan;               // 0 = left, 255 = right
    uint8_t  pad[2];            // pad to 24 bytes
} mixer_t;

typedef struct
{
    // mixer variables
    mixer_t  mixer;

    // effect memory
    uint8_t  last_volume;
    uint8_t  last_effect;
    uint8_t  last_param;
    uint8_t  last_sample;

    // song variables
    uint8_t  vibrato_speed;
    uint8_t  vibrato_depth;
    int8_t   vibrato_position;
    uint8_t  vibrato_wavecontrol;

    uint8_t  tremolo_speed;
    uint8_t  tremolo_depth;
    int8_t   tremolo_position;
    uint8_t  tremolo_wavecontrol;

    int16_t  period;
    uint8_t  note;
    uint8_t  finetune;

    int16_t  porta_period;
    uint8_t  porta_speed;

    uint8_t  keyon;
    uint16_t fadeout;
    uint16_t fadeout_step;

#ifdef USE_VOL_ENVELOPE
    uint8_t  venv_a;
    uint8_t  venv_b;
    uint16_t venv_step;

    uint16_t venv_pts[24];
    uint8_t  num_venv_pts;
    uint8_t  venv_sust_pt;
    uint8_t  venv_loop_start;
    uint8_t  venv_loop_end;
    uint8_t  venv_type;
#ifdef __32X__
    uint8_t  pad[7]; // pad to multiple of 16
#endif
#endif
} channel_t;

typedef struct
{
    uint8_t state;              // playing paused whatever
    int8_t  fade;               // if 0, then no fade. if > 0 then fade global volume by this amount
    int8_t  volume;             // global volume for song (0 to 64)

    uint8_t mod;                // module id
    uint8_t loop;               // loop when finished
    uint8_t restart;            // restart position for loop

#ifdef __32X__
    uint8_t pad1[10];           // bump rest out of shared state range (one cache line)
#endif

    const uint8_t *order;       // pointer to ROM table of orders
    const pattern_t *pattern;   // pointer to current pattern array
    const uint8_t *pattern_data;// shortcut to the data

    uint8_t current_pattern;
    uint8_t max_order;          // where the song ends
    uint8_t order_index;        // where we are in the xm order
    uint8_t ticks;              // ticking

    uint8_t current_row;        // when this reaches pattern.row, then go to the next row
    uint8_t speed;              // current speed - ticks per row
    uint8_t tempo;              // current tempo in BPM

#ifdef __32X__
    uint8_t  pad2[12];          // pad to multiple of 16 bytes
#endif

    uint8_t channels;           // number channels
    channel_t channel[1];       // make channels part of song to allow easy nesting
} song_t;

#define XMP_PLAYING 0x80
#define XMP_PAUSED  0x01
#define XMP_STOPPED 0x00

#define XMP_LOOP_FOREVER 0xFF

#define XMP_MAX_VOLUME 64

#define XMP_PAN_LEFT   0
#define XMP_PAN_CENTER 128
#define XMP_PAN_RIGHT  255

#ifdef __cplusplus
extern "C" {
#endif

// start playing a song, 0 to play once, N to loop N times (255 = forever)
song_t *xmp_start_song(uint8_t mod, uint8_t loop);

// stop song
void xmp_stop_song(song_t *song);

// is the song still playing?
uint8_t xmp_is_playing(song_t *song);

// pass 1 to pause, 0 to unpause (only if playing, else does nothing)
void xmp_pause(song_t *song, uint8_t pause);

// is the song paused?
uint8_t xmp_is_paused(song_t *song);

// set song fade out
void xmp_set_fade(song_t *song, int8_t fade);

// get song tempo
uint8_t xmp_get_tempo(song_t *song);

// get number channels in song
uint8_t xmp_num_channels(song_t *song);

// get song channel
channel_t *xmp_get_channel(song_t *song, uint8_t chan);

// process channel volume envelope
void xmp_proc_vol_envelope(channel_t *chan);

// get channel volume envelope value
int8_t xmp_get_vol_envelope(channel_t *chan);

// get channel volume fadeout value
int8_t xmp_get_vol_fadeout(channel_t *chan, int8_t scale);

// get song volume scale
int8_t xmp_get_scale(song_t *song, int8_t master_vol);

// call from mixer at tempo rate
void xmp_update(song_t *song);

#ifdef __cplusplus
}
#endif

#endif
