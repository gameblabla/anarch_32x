#include <stdlib.h>
#include <string.h>

#include "xmp.h"
#include "tables.h"

#ifdef USE_MALLOC
#define xmp_malloc malloc
#else
extern void *xmp_malloc(int size);
#endif

extern const mod_t *const song_table[];

// From Amiga ProTracker playroutine source code
//
//  for Vibrato and Tremolo
//
static const uint8_t SineTable[32] = {
    0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
    255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24
};

typedef struct
{
  int32_t channel;
  int32_t note;
  int32_t relative;
  int32_t finetune;
  int32_t volume;
  int32_t effect;
  int32_t param;
} note_t;

/* EFFECTS */
#define EFFECT_ARPEGGIO               ( (uint8_t) 0x00 )   /* Effect 0xy (Arpeggio) */
#define EFFECT_PORTA_UP               ( (uint8_t) 0x01 )   /* Effect 1xy (Porta Up) */
#define EFFECT_PORTA_DOWN             ( (uint8_t) 0x02 )   /* Effect 2xy (Porta Down) */
#define EFFECT_PORTA_TO_NOTE          ( (uint8_t) 0x03 )   /* Effect 3xy (Porta To Note) */
#define EFFECT_VIBRATO                ( (uint8_t) 0x04 )   /* Effect 4xy (Vibrato) */
#define EFFECT_PORTA_PLUS_VOL_SLIDE   ( (uint8_t) 0x05 )   /* Effect 5xy (Porta + Vol Slide) */
#define EFFECT_VIBRATO_PLUS_VOL_SLIDE ( (uint8_t) 0x06 )   /* Effect 6xy (Vibrato + Vol Slide) */
#define EFFECT_TREMOLO                ( (uint8_t) 0x07 )   /* Effect 7xy (Tremolo) */
#define EFFECT_PAN                    ( (uint8_t) 0x08 )   /* Effect 8xy (Pan) */
#define EFFECT_SAMPLE_OFFSET          ( (uint8_t) 0x09 )   /* Effect 9xy (Sample Offset) */
#define EFFECT_VOLUME_SLIDE           ( (uint8_t) 0x0A )   /* Effect Axy (Volume Slide) */
#define EFFECT_JUMP_TO_PATTERN        ( (uint8_t) 0x0B )   /* Effect Bxy (Jump To Pattern) */
#define EFFECT_SET_VOLUME             ( (uint8_t) 0x0C )   /* Effect Cxy (Set Volume) */
#define EFFECT_PATTERN_BREAK          ( (uint8_t) 0x0D )   /* Effect Dxy (Pattern Break) */
#define EFFECT_EXTENDED_EFFECTS       ( (uint8_t) 0x0E )   /* Extended Effects */
#define EFFECT_SET_SPEED              ( (uint8_t) 0x0F )   /* Effect Fxy (Set Speed) */
// new effects added with XM format
#define EFFECT_SET_GLOBAL_VOLUME      ( (uint8_t) 0x10 )   /* Effect G (Set Global Volume) */
#define EFFECT_GLOBAL_VOLUME_SLIDE    ( (uint8_t) 0x11 )   /* Effect H (Global Volume Slide) */
#define EFFECT_KEY_OFF                ( (uint8_t) 0x14 )   /* Effect K (Key Off) */
#define EFFECT_SET_ENVELOPE_POS       ( (uint8_t) 0x15 )   /* Effect L (Set Envelope Position) */
#define EFFECT_PANNING_SLIDE          ( (uint8_t) 0x19 )   /* Effect P (Panning Slide) */
#define EFFECT_MULTI_RETRIG_NOTE      ( (uint8_t) 0x1B )   /* Effect R (Multi Retrig Note) */
#define EFFECT_TREMOR                 ( (uint8_t) 0x1D )   /* Effect T (Tremor) */
#define EFFECT_EXTRA_FINE_PORTA_UP    ( (uint8_t) 0x21 )   /* Effect X1 (Extra Fine Porta Up) */
#define EFFECT_EXTRA_FINE_PORTA_DOWN  ( (uint8_t) 0x21 )   /* Effect X2 (Extra Fine Porta Down) */

#define EFFECT_SET_FILTER             ( (uint8_t) 0x00 )   /* Effect E0x (Set Filter - only on the Amiga) */
#define EFFECT_FINE_PORTA_UP          ( (uint8_t) 0x01 )   /* Effect E1x (Fine Porta Up) */
#define EFFECT_FINE_PORTA_DOWN        ( (uint8_t) 0x02 )   /* Effect E2x (Fine Porta Down) */
#define EFFECT_GLISSANDO_CONTROL      ( (uint8_t) 0x03 )   /* Effect E3x (Glissando Control) */
#define EFFECT_SET_VIBRATO_WAVEFORM   ( (uint8_t) 0x04 )   /* Effect E4x (Set Vibrato Waveform) */
#define EFFECT_SET_FINETUNE           ( (uint8_t) 0x05 )   /* Effect E5x (Set Finetune) */
#define EFFECT_PATTERN_LOOP           ( (uint8_t) 0x06 )   /* Effect E6x (Pattern Loop) */
#define EFFECT_SET_TREMOLO_WAVEFORM   ( (uint8_t) 0x07 )   /* Effect E7x (Set Tremolo WaveForm) */
#define EFFECT_POSITION_PANNING       ( (uint8_t) 0x08 )   /* Effect E8x (Position Panning) */
#define EFFECT_RETRIG_NOTE            ( (uint8_t) 0x09 )   /* Effect E9x (Retrig Note) */
#define EFFECT_FINE_VOLUME_SLIDE_UP   ( (uint8_t) 0x0A )   /* Effect EAx (Fine Volume Slide Up) */
#define EFFECT_FINE_VOLUME_SLIDE_DOWN ( (uint8_t) 0x0B )   /* Effect EBx (Fine Volume Slide Down) */
#define EFFECT_CUT_NOTE               ( (uint8_t) 0x0C )   /* Effect ECx (Cut Note) */
#define EFFECT_DELAY_NOTE             ( (uint8_t) 0x0D )   /* Effect EDx (Delay Note) */
#define EFFECT_PATTERN_DELAY          ( (uint8_t) 0x0E )   /* Effect EEx (Pattern Delay) */
#define EFFECT_INVERT_LOOP            ( (uint8_t) 0x0F )   /* Effect EFx (Invert Loop) */

// handle tick 0 notes/effects
static void xm_note(song_t *song, note_t *xmnote)
{
    channel_t *chan = &song->channel[xmnote->channel];
    mixer_t *mix = &chan->mixer;

    if (xmnote->note)
    {
        if (xmnote->note != 97)
        {
            int32_t c, np;

            // note-on
            c = (xmnote->note - 1) + xmnote->relative;
            if (c < 0)
                c = 0;
            else if (c > 118)
                c = 118;
            np = (10 * 12 * 64) - ((c + 12) * 64) - (xmnote->finetune / 2);
            if (np < 0)
                np = 0;
            else if (np > 8191)
                np = 8191;
            if (xmnote->effect == EFFECT_PORTA_TO_NOTE || xmnote->effect == EFFECT_PORTA_PLUS_VOL_SLIDE)
            {
                // porta target note period
                chan->porta_period = np;
            }
            else
            {
                // new note
                chan->period = np;
                chan->note = c;
                chan->finetune = xmnote->finetune;
                mix->increment = lfr_table[np];
                mix->position = 0;
                if (chan->vibrato_wavecontrol < 4)
                    chan->vibrato_position = 0;
                if (chan->tremolo_wavecontrol < 4)
                    chan->tremolo_position = 0;
#ifdef USE_VOL_ENVELOPE
                chan->venv_a = 0;
                chan->venv_b = 1;
                chan->venv_step = 0;
#endif
                chan->keyon = 1; // note-on
                chan->fadeout = 65535; // should be 65536, but fuck it - we're in key-off for fadeout
            }
        }
        else
        {
            // note-off
            chan->keyon = 0;
        }
    }

    // process effects - volume channel effects first
    if (xmnote->volume)
    {
        if (xmnote->volume <= 0x50)
        {
            // set volume
            mix->volume = xmnote->volume - 0x10;
            if (mix->volume < 0)
                mix->volume = 0; // safety check
        }
        else
        {
            int16_t tmp;
            uint8_t vp = xmnote->volume & 0x0F;

            switch (xmnote->volume >> 4)
            {
                case 6:
                    // volume slide down (every tick)
                    chan->last_volume = xmnote->volume;
                    mix->volume = mix->volume - vp;
                    if (mix->volume < 0)
                        mix->volume = 0;
                    break;
                case 7:
                    // volume slide up (every tick)
                    chan->last_volume = xmnote->volume;
                    mix->volume = mix->volume + vp;
                    if (mix->volume > 64)
                        mix->volume = 64;
                    break;
                case 8:
                    // fine volume slide down (once per row)
                    chan->last_volume = 0;
                    mix->volume = mix->volume - vp;
                    if (mix->volume < 0)
                        mix->volume = 0;
                    break;
                case 9:
                    // fine volume slide up (once per row)
                    chan->last_volume = 0;
                    mix->volume = mix->volume + vp;
                    if (mix->volume > 64)
                        mix->volume = 64;
                    break;
                case 10:
                    // set vibrato speed
                    chan->last_volume = xmnote->volume;
                    chan->vibrato_speed = vp;
                    break;
                case 11:
                    // vibrato
                    chan->last_volume = xmnote->volume;
                    chan->vibrato_depth = vp;
                    break;
                case 12:
                    // set panning
                    chan->last_volume = 0;
                    mix->pan = (vp << 4) | vp;
                    break;
                case 13:
                    // panning slide left (every tick)
                    chan->last_volume = xmnote->volume;
                    tmp = (int16_t)mix->pan - vp;
                    mix->pan = (tmp >= 0) ? tmp : 0;
                    break;
                case 14:
                    // panning slide right (every tick)
                    chan->last_volume = xmnote->volume;
                    tmp = (int16_t)mix->pan + vp;
                    mix->pan = (tmp < 256) ? tmp : 255;
                    break;
                case 15:
                    // tone porta
                    chan->last_volume = xmnote->volume;
                    chan->porta_speed = (vp << 4) | vp;
                    break;
            }
        }
    }

    // now all other effects
    if (xmnote->effect)
    {
        const int8_t finetable[16] = {
            -128, -112, -96, -80, -64, -48, -32, -16,
            0, 16, 32, 48, 64, 80, 96, 112
        };
        const int8_t effectmem[0x22] = {
            0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 1
        };
        const int8_t extendedmem[0x10] = {
            0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
        };
        uint8_t p, x, y;
        int32_t np;

        if (chan->last_effect == xmnote->effect)
        {
            if (xmnote->effect != EFFECT_EXTENDED_EFFECTS)
            {
                if (effectmem[xmnote->effect])
                {
                    if (xmnote->param)
                        chan->last_param = xmnote->param;
                }
                else
                {
                    chan->last_param = xmnote->param;
                }
            }
            else
            {
                if (extendedmem[xmnote->param >> 4])
                {
                    if (xmnote->param & 0x0F)
                        chan->last_param = xmnote->param;
                }
                else
                {
                    chan->last_param = xmnote->param;
                }
            }
        }
        else
        {
            chan->last_effect = xmnote->effect;
            chan->last_param = xmnote->param;
        }

        p = chan->last_param;
        x = p >> 4;
        y = p & 0x0F;

        // effects that are processed on tick 0
        switch (xmnote->effect)
        {
            case EFFECT_PORTA_TO_NOTE:
                chan->porta_speed = p;
                break;

            case EFFECT_VIBRATO:
                if (x)
                    chan->vibrato_speed = x;
                if (y)
                    chan->vibrato_depth = y;
                break;

            case EFFECT_PORTA_PLUS_VOL_SLIDE:
                // volume slide every tick
                if (x)
                {
                    mix->volume = mix->volume + x;
                    if (mix->volume > 64)
                        mix->volume = 64;
                }
                if (y)
                {
                    mix->volume = mix->volume - y;
                    if (mix->volume < 0)
                        mix->volume = 0;
                }
                break;

            case EFFECT_VIBRATO_PLUS_VOL_SLIDE:
                // volume slide every tick
                if (x)
                {
                    mix->volume = mix->volume + x;
                    if (mix->volume > 64)
                        mix->volume = 64;
                }
                if (y)
                {
                    mix->volume = mix->volume - y;
                    if (mix->volume < 0)
                        mix->volume = 0;
                }
                break;

            case EFFECT_TREMOLO:
                if (x)
                    chan->tremolo_speed = x;
                if (y)
                    chan->tremolo_depth = y;
                break;

            case EFFECT_PAN:
                mix->pan = p;
                break;

            case EFFECT_SAMPLE_OFFSET:
                mix->position = (uint32_t)p << (8+14);
                break;

            case EFFECT_VOLUME_SLIDE:
                // volume slide every tick
                if (x)
                {
                    mix->volume = mix->volume + x;
                    if (mix->volume > 64)
                        mix->volume = 64;
                }
                if (y)
                {
                    mix->volume = mix->volume - y;
                    if (mix->volume < 0)
                        mix->volume = 0;
                }
                break;

            case EFFECT_JUMP_TO_PATTERN:
                song->current_row = 0;
                song->order_index = p;
                song->current_pattern = song->order[song->order_index];
                song->pattern_data = song->pattern[song->current_pattern].data;
                break;

            case EFFECT_SET_VOLUME:
                mix->volume = p;
                if (mix->volume > 64)
                    mix->volume = 64;
                break;

            case EFFECT_PATTERN_BREAK:
                song->current_row = x*10 + y;
                if (song->current_row >= song->pattern[song->current_pattern].rows)
                    song->current_row = 0;
                song->order_index++;
                if (song->order_index >= song->max_order)
                {
                    song->state = XMP_STOPPED;
                    if (song->loop)
                    {
                        int32_t ch;

                        if (song->loop != XMP_LOOP_FOREVER)
                            song->loop--;

                        song->current_row = 0;
                        song->order_index = song->restart;
                        song->speed = song_table[song->mod]->speed;
                        song->tempo = song_table[song->mod]->tempo;
                        song->state = XMP_PLAYING;
                        song->current_pattern = song->order[song->order_index];
                        song->pattern_data = song->pattern[song->current_pattern].data;
                        song->ticks = song_table[song->mod]->speed; // force immediate processing

                        for (ch = 0; ch < song->channels; ch++)
                            memset(&song->channel[ch], 0, sizeof(channel_t));
                    }
                    return;
                }
                else
                {
                    int32_t ix, iy;
                    song->current_pattern = song->order[song->order_index];
                    song->pattern_data = song->pattern[song->current_pattern].data;
                    // parse data row times here
                    for (ix=0; ix<song->current_row; ix++)
                    {
                        for (iy=0; iy<song->channels; iy++)
                        {
                            uint8_t note = 0, sample = 0, volume = 0, effect = 0, param = 0;

                            // unpack pattern data using XM format
                            uint8_t what = *song->pattern_data++;
                            if (what)
                            {
                                if (what & 0x80)
                                {
                                    // compressed - what == flags
                                    if (what & 1)
                                        note = *song->pattern_data++;
                                    if (what & 2)
                                        sample = *song->pattern_data++;
                                    if (what & 4)
                                        volume = *song->pattern_data++;
                                    if (what & 8)
                                    {
                                        effect = *song->pattern_data++;
                                        param = *song->pattern_data++;
                                    }
                                }
                                else
                                {
                                    // not compressed, what == note
                                    note = what;
                                    sample = *song->pattern_data++;
                                    volume = *song->pattern_data++;
                                    effect = *song->pattern_data++;
                                    param = *song->pattern_data++;
                                }
                            }
                            else
                                break;
                        }
                    }
                }
                break;

            case EFFECT_EXTENDED_EFFECTS:
                switch (x)
                {
                    case EFFECT_FINE_PORTA_UP:
                        // once per row
                        chan->period -= y*3;
                        mix->increment = lfr_table[chan->period];
                        break;

                    case EFFECT_FINE_PORTA_DOWN:
                        // once per row
                        chan->period += y*3;
                        mix->increment = lfr_table[chan->period];
                        break;

                    case EFFECT_SET_VIBRATO_WAVEFORM:
                        chan->vibrato_wavecontrol = y;
                        break;

                    case EFFECT_SET_FINETUNE:
                        chan->finetune = finetable[y];
                        np = (10 * 12 * 64) - ((chan->note + 12) * 64) - (chan->finetune / 2);
                        if (np < 0)
                            np = 0;
                        else if (np > 8191)
                            np = 8191;
                        chan->period = np;
                        mix->increment = lfr_table[chan->period];
                        break;

                    case EFFECT_SET_TREMOLO_WAVEFORM:
                        chan->tremolo_wavecontrol = y;
                        break;

                    case EFFECT_POSITION_PANNING:
                        mix->pan = (y << 4) | y;
                        break;

                    case EFFECT_FINE_VOLUME_SLIDE_UP:
                        // once per row
                        mix->volume += y;
                        if (mix->volume > 64)
                            mix->volume = 64;
                        break;

                    case EFFECT_FINE_VOLUME_SLIDE_DOWN:
                        // once per row
                        mix->volume -= y;
                        if (mix->volume < 0)
                            mix->volume = 0;
                        break;

                    case EFFECT_CUT_NOTE:
                        if (y == 0)
                        {
                            chan->keyon = 0;
                            mix->volume = 0; // silence channel
                        }
                        break;

                    case EFFECT_DELAY_NOTE:
                        if (y != 0)
                            mix->volume = 0; // silence channel
                        break;

                }
                break;

            case EFFECT_SET_SPEED:
                if (p >= 0x20)
                    song->tempo = p;
                else
                    song->speed = p;
                break;

            case EFFECT_SET_GLOBAL_VOLUME:
                song->volume = p;
                if (song->volume > 64)
                    song->volume = 64;
                break;

            case EFFECT_GLOBAL_VOLUME_SLIDE:
                // volume slide every tick
                if (x)
                {
                    song->volume = song->volume + x;
                    if (song->volume > 64)
                        song->volume = 64;
                }
                if (y)
                {
                    song->volume = song->volume - y;
                    if (song->volume < 0)
                        song->volume = 0;
                }
                break;

            case EFFECT_KEY_OFF:
                if (p == 0)
                {
                    // note-off
                    chan->keyon = 0;
                }
                break;

        }
    }
}

// process row of pattern
static void xm_row(song_t *song)
{
    int32_t ch;

    for (ch = 0; ch < song->channels; ch++)
    {
        uint8_t note = 0, sample = 0, volume = 0, effect = 0, param = 0;

        // unpack pattern data using XM format
        uint8_t what = *song->pattern_data++;
        if (what)
        {
            if (what & 0x80)
            {
                // compressed - what == flags
                if (what & 1)
                    note = *song->pattern_data++;
                if (what & 2)
                    sample = *song->pattern_data++;
                if (what & 4)
                    volume = *song->pattern_data++;
                if (what & 8)
                {
                    effect = *song->pattern_data++;
                    param = *song->pattern_data++;
                }
            }
            else
            {
                // not compressed, what == note
                note = what;
                sample = *song->pattern_data++;
                volume = *song->pattern_data++;
                effect = *song->pattern_data++;
                param = *song->pattern_data++;
            }
        }
        else
            break;

        if (note || effect || volume)
        {
            note_t n = {
                ch,
                note,
                sample ? song_table[song->mod]->sample[sample-1].relative : 0,
                sample ? song_table[song->mod]->sample[sample-1].finetune : 0,
                volume,
                effect,
                param
            };
            if (sample)
            {
                if (sample != song->channel[ch].last_sample)
                {
                    // change instrument
#ifdef USE_VOL_ENVELOPE
                    memcpy(&song->channel[ch].venv_pts, &song_table[song->mod]->sample[sample-1].venv_pts, 48);
                    song->channel[ch].num_venv_pts = song_table[song->mod]->sample[sample-1].num_venv_pts;
                    song->channel[ch].venv_sust_pt = song_table[song->mod]->sample[sample-1].venv_sust_pt;
                    song->channel[ch].venv_loop_start = song_table[song->mod]->sample[sample-1].venv_loop_start;
                    song->channel[ch].venv_loop_end = song_table[song->mod]->sample[sample-1].venv_loop_end;
                    song->channel[ch].venv_type = song_table[song->mod]->sample[sample-1].venv_type;
#endif
                    song->channel[ch].fadeout_step = song_table[song->mod]->sample[sample-1].fadeout;
                    song->channel[ch].last_sample = sample;
                }
                song->channel[ch].mixer.data = song_table[song->mod]->sample[sample-1].data;
                if (song_table[song->mod]->sample[sample-1].loop_length)
                    song->channel[ch].mixer.length = (song_table[song->mod]->sample[sample-1].loop_start + song_table[song->mod]->sample[sample-1].loop_length)<<14;
                else
                    song->channel[ch].mixer.length = song_table[song->mod]->sample[sample-1].length<<14;
                song->channel[ch].mixer.loop_length = song_table[song->mod]->sample[sample-1].loop_length<<14;
                song->channel[ch].mixer.volume = song_table[song->mod]->sample[sample-1].vol;
                song->channel[ch].mixer.pan = song_table[song->mod]->sample[sample-1].pan;
            }
            xm_note(song, &n);
        }
    }
}

// update vibrato effect
static void do_vibrato(song_t *song, channel_t *chan)
{
    uint16_t Delta = 0;
    uint8_t Temp;
    mixer_t *mix = &chan->mixer;

    Temp = chan->vibrato_position & 31; // Temp will be the index

    switch (chan->vibrato_wavecontrol & 3)
    {
        case 0:
            Delta = SineTable[Temp]; // look up sine table
            break;

        case 1:
            Temp <<= 3; // ramp down
            if (chan->vibrato_position < 0)
                Temp = 255 - Temp;
            Delta = Temp;
            break;

        case 2:
            Delta = 255; // square
            break;

        case 3:
            Delta = rand() & 255; // random
            break;
    }

    Delta *= chan->vibrato_depth * 3;
    Delta >>= 7;

    if (chan->vibrato_position >= 0)
    {
        mix->increment = lfr_table[chan->period - Delta];
    }
    else
    {
        mix->increment = lfr_table[chan->period + Delta];
    }

    chan->vibrato_position += chan->vibrato_speed;
    if (chan->vibrato_position > 31)
        chan->vibrato_position -= 64;
}

// update tremolo effect
static void do_tremolo(song_t *song, channel_t *chan)
{
    uint16_t Delta;
    uint8_t Temp;
    mixer_t *mix = &chan->mixer;

    Temp = chan->tremolo_position & 31; // Temp will be the index

    switch (chan->tremolo_wavecontrol & 3)
    {
        case 0:
            Delta = SineTable[Temp]; // look up sine table
            break;

        case 1:
            Temp <<= 3; // ramp down
            if (chan->tremolo_position < 0)
                Temp = 255 - Temp;
            Delta = Temp;
            break;

        case 2:
            Delta = 255; // square
            break;

        case 3:
            Delta = rand() & 255; // random
            break;
    }

    Delta *= chan->tremolo_depth;
    Delta >>= 6;

    if (chan->tremolo_position >= 0)
    {
        mix->volume += Delta;
        if (mix->volume > 64)
            mix->volume = 64;
    }
    else
    {
        mix->volume -= Delta;
        if (mix->volume < 0)
            mix->volume = 0;
    }

    chan->tremolo_position += chan->tremolo_speed;
    if (chan->tremolo_position > 31)
        chan->tremolo_position -= 64;
}


//update porta effect
static void do_porta(song_t *song, channel_t *chan)
{
    mixer_t *mix = &chan->mixer;

    if (chan->period < chan->porta_period)
    {
        // slide pitch down
        chan->period += chan->porta_speed*3;
        if (chan->period >= chan->porta_period)
        {
            // reached target
            chan->period = chan->porta_period;
        }
        mix->increment = lfr_table[chan->period];
    }
    else if (chan->period > chan->porta_period)
    {
        // slide pitch up
        chan->period -= chan->porta_speed*3;
        if (chan->period <= chan->porta_period)
        {
            // reached target
            chan->period = chan->porta_period;
        }
        mix->increment = lfr_table[chan->period];
    }
}

// update tick based effects
static void xm_update(song_t *song)
{
    int32_t ch;

    for (ch = 0; ch < song->channels; ch++)
    {
        channel_t *chan = &song->channel[ch];
        mixer_t *mix = &chan->mixer;

        // process effects - volume channel effects first
        if (chan->last_volume)
        {
            int16_t tmp;
            uint8_t vp = chan->last_volume & 0x0F;

            switch (chan->last_volume >> 4)
            {
                case 6:
                    // volume slide down (every tick)
                    mix->volume = mix->volume - vp;
                    if (mix->volume < 0)
                        mix->volume = 0;
                    break;
                case 7:
                    // volume slide up (every tick)
                    mix->volume = mix->volume + vp;
                    if (mix->volume > 64)
                        mix->volume = 64;
                    break;
                case 10:
                    // set vibrato speed
                case 11:
                    // vibrato
                    if (chan->last_effect != EFFECT_VIBRATO && chan->last_effect != EFFECT_VIBRATO_PLUS_VOL_SLIDE)
                        do_vibrato(song, chan);
                    break;
                case 13:
                    // panning slide left (every tick)
                    tmp = (int16_t)mix->pan - vp;
                    mix->pan = (tmp >= 0) ? tmp : 0;
                    break;
                case 14:
                    // panning slide right (every tick)
                    tmp = (int16_t)mix->pan + vp;
                    mix->pan = (tmp < 256) ? tmp : 255;
                    break;
                case 15:
                    // tone porta
                    if (chan->last_effect != EFFECT_PORTA_TO_NOTE && chan->last_effect != EFFECT_PORTA_PLUS_VOL_SLIDE)
                        do_porta(song, chan);
                    break;
            }
        }

        // effects that are processed every tick
        if (chan->last_effect)
        {
            uint8_t p, x, y;
            p = chan->last_param;
            x = p >> 4;
            y = p & 0x0F;

            switch (chan->last_effect)
            {
                case EFFECT_ARPEGGIO:
                    switch (song->ticks % 3)
                    {
                        case 0:
                            mix->increment = lfr_table[chan->period];
                            break;

                        case 1:
                            mix->increment = lfr_table[chan->period + (x << 6)];
                            break;

                        case 2:
                            mix->increment = lfr_table[chan->period + (y << 6)];
                            break;
                    }
                    break;

                case EFFECT_PORTA_UP:
                    if (p)
                    {
                        chan->period -= p*3;
                        mix->increment = lfr_table[chan->period];
                    }
                    break;

                case EFFECT_PORTA_DOWN:
                    if (p)
                    {
                        chan->period += p*3;
                        mix->increment = lfr_table[chan->period];
                    }
                    break;

                case EFFECT_PORTA_TO_NOTE:
                    do_porta(song, chan);
                    break;

                case EFFECT_VIBRATO:
                    do_vibrato(song, chan);
                    break;

                case EFFECT_PORTA_PLUS_VOL_SLIDE:
                    do_porta(song, chan);
                    // volume slide every tick
                    if (x)
                    {
                        mix->volume = mix->volume + x;
                        if (mix->volume > 64)
                            mix->volume = 64;
                    }
                    if (y)
                    {
                        mix->volume = mix->volume - y;
                        if (mix->volume < 0)
                            mix->volume = 0;
                    }
                    break;

                case EFFECT_VIBRATO_PLUS_VOL_SLIDE:
                    do_vibrato(song, chan);
                    // volume slide every tick
                    if (x)
                    {
                        mix->volume = mix->volume + x;
                        if (mix->volume > 64)
                            mix->volume = 64;
                    }
                    if (y)
                    {
                        mix->volume = mix->volume - y;
                        if (mix->volume < 0)
                            mix->volume = 0;
                    }
                    break;

                case EFFECT_TREMOLO:
                    do_tremolo(song, chan);
                    break;

                case EFFECT_VOLUME_SLIDE:
                    // volume slide every tick
                    if (x)
                    {
                        mix->volume = mix->volume + x;
                        if (mix->volume > 64)
                            mix->volume = 64;
                    }
                    if (y)
                    {
                        mix->volume = mix->volume - y;
                        if (mix->volume < 0)
                            mix->volume = 0;
                    }
                    break;

                case EFFECT_EXTENDED_EFFECTS:
                    switch (x)
                    {
                        case EFFECT_RETRIG_NOTE:
                            if (y && !(song->ticks % y))
                            {
                                int32_t np = (10 * 12 * 64) - ((chan->note + 12) * 64) - (chan->finetune / 2);
                                if (np < 0)
                                    np = 0;
                                else if (np > 8191)
                                    np = 8191;
                                mix->increment = lfr_table[np];
                                mix->position = 0;
                                chan->period = np;
                                if (chan->vibrato_wavecontrol < 4)
                                    chan->vibrato_position = 0;
                                if (chan->tremolo_wavecontrol < 4)
                                    chan->tremolo_position = 0;
                                chan->keyon = 1; // key-on
                                chan->fadeout = 65535; // should be 65536, but fuck it - we're in key-off for that
                                if (chan->last_sample)
                                {
                                    mix->data = song_table[song->mod]->sample[chan->last_sample-1].data;
                                    if (song_table[song->mod]->sample[chan->last_sample-1].loop_length)
                                        mix->length = (song_table[song->mod]->sample[chan->last_sample-1].loop_start + song_table[song->mod]->sample[chan->last_sample-1].loop_length)<<14;
                                    else
                                        mix->length = song_table[song->mod]->sample[chan->last_sample-1].length<<14;
                                    mix->loop_length = song_table[song->mod]->sample[chan->last_sample-1].loop_length<<14;
                                    mix->volume = song_table[song->mod]->sample[chan->last_sample-1].vol;
                                    mix->pan = song_table[song->mod]->sample[chan->last_sample-1].pan;
                               }
                            }
                            break;

                        case EFFECT_CUT_NOTE:
                            if (y == song->ticks)
                            {
                                chan->keyon = 0;
                                mix->volume = 0; // silence channel
                            }
                            break;

                        case EFFECT_DELAY_NOTE:
                            if (y == song->ticks)
                                if (chan->last_sample)
                                    mix->volume = song_table[song->mod]->sample[chan->last_sample-1].vol; // note-on
                            break;

                    }
                    break;

                case EFFECT_GLOBAL_VOLUME_SLIDE:
                    // volume slide every tick
                    if (x)
                    {
                        song->volume = song->volume + x;
                        if (song->volume > 64)
                            song->volume = 64;
                    }
                    if (y)
                    {
                        song->volume = song->volume - y;
                        if (song->volume < 0)
                            song->volume = 0;
                    }
                    break;

                case EFFECT_KEY_OFF:
                    if (p == song->ticks)
                    {
                        // note-off
                        chan->keyon = 0;
                    }
                    break;

            }
        }

    }
}

//------------------------------------------------------------------------------
// start playing a song, 0 to play once, N to loop N times (255 = forever)
song_t *xmp_start_song(uint8_t mod, uint8_t loop)
{
    song_t *s;
    int32_t i;

    s = xmp_malloc((song_table[mod]->channels - 1) * sizeof(channel_t) + sizeof(song_t));
    if (!s)
        return s; // couldn't allocate memory for song structure

    s->mod = mod;
    s->loop = loop;
    s->restart = song_table[mod]->restart;
    s->order = song_table[mod]->order;
    s->pattern = song_table[mod]->pattern;
    s->order_index = 0;
    s->current_row = 0;
    s->speed = song_table[mod]->speed;
    s->tempo = song_table[mod]->tempo;
    s->state = XMP_PLAYING;
    s->current_pattern = s->order[s->order_index];
    s->pattern_data = song_table[mod]->pattern[s->current_pattern].data;
    s->ticks = song_table[mod]->speed; // force immediate processing
    s->max_order = song_table[mod]->song_length;
    s->channels = song_table[mod]->channels;
    s->fade = 0;
    s->volume = XMP_MAX_VOLUME;

    for (i = 0; i < s->channels; i++)
        memset(&s->channel[i], 0, sizeof(channel_t));

    return s;
}

// stop song
void xmp_stop_song(song_t *song)
{
    if (song)
        song->state = XMP_STOPPED;
}

// is the song still playing?
uint8_t xmp_is_playing(song_t *song)
{
    if (song)
        return song->state & XMP_PLAYING ? 1 : 0;

    return 0;
}

// pass 1 to pause, 0 to unpause (only if playing, else does nothing)
void xmp_pause(song_t *song, uint8_t pause)
{
    if (song)
        if (song->state & XMP_PLAYING)
            song->state = (song->state & ~XMP_PAUSED) | pause;
}

// is the song paused?
uint8_t xmp_is_paused(song_t *song)
{
    if (song)
        return song->state & XMP_PAUSED;

    return 0;
}

// set song fade out
void xmp_set_fade(song_t *song, int8_t fade)
{
    if (song)
        song->fade = fade;
}

// get song tempo
uint8_t xmp_get_tempo(song_t *song)
{
    if (song)
        return song->tempo;

    return 0;
}

// get number channels in song
uint8_t xmp_num_channels(song_t *song)
{
    if (song)
        return song->channels;

    return 0;
}

// get song channel
channel_t *xmp_get_channel(song_t *song, uint8_t chan)
{
    if (song)
        return &song->channel[chan];

    return (channel_t *)NULL;
}

// process channel volume envelope
void xmp_proc_vol_envelope(channel_t *chan)
{
#ifdef USE_VOL_ENVELOPE
    if (chan->venv_type & 1)
    {
        // check if reached sustain part of segment
        if (chan->keyon && (chan->venv_type & 2) && (chan->venv_a == chan->venv_sust_pt) && (chan->venv_step == chan->venv_pts[chan->venv_a*2]))
            return; // sustaining

        // not sustaining - next step in segment
        if ((chan->venv_step != chan->venv_pts[chan->venv_b*2]) && (chan->venv_b < chan->num_venv_pts))
            chan->venv_step++;
        // check if segment done
        if (chan->venv_step == chan->venv_pts[chan->venv_b*2])
        {
            // check loop segment
            if (chan->venv_type & 4)
            {
                if (chan->venv_b == chan->venv_loop_end)
                {
                    chan->venv_a = chan->venv_loop_start;
                    chan->venv_b = chan->venv_loop_start+1;
                    chan->venv_step = chan->venv_pts[chan->venv_a*2];
                    return;
                }
            }
            // next segment if more points
            if (chan->venv_b < chan->num_venv_pts - 1)
            {
                chan->venv_a++;
                chan->venv_b++;
            }
        }
    }
#endif
}

// get channel volume envelope value
int8_t xmp_get_vol_envelope(channel_t *chan)
{
#ifdef USE_VOL_ENVELOPE
    if (chan->venv_type & 1)
    {
        int32_t env, step, dx, t, y0, y1;

        step = chan->venv_step;
        if (step > chan->venv_pts[chan->venv_b*2])
            step = chan->venv_pts[chan->venv_b*2]; // safety check - don't go past end of segment
        dx = (chan->venv_pts[chan->venv_b*2] - chan->venv_pts[chan->venv_a*2]); // segment time delta (in ticks)
        if (!dx)
            dx = 1;
        t = (chan->venv_pts[chan->venv_b*2] - step) * 65536 / dx; // amount of way across delta (65536 = all the way)
        y0 = chan->venv_pts[chan->venv_a*2+1]; // start segment volume
        y1 = chan->venv_pts[chan->venv_b*2+1]; // end segment volume
        env = (uint32_t)((y0 * t) + (y1 * (65536 - t))) >> 16; // volume at current place along segment
        return env;
    }
#endif
    // no volume envelope for this channel
    return -1;
}

// get channel volume fadeout value and scale the channel volume scale
int8_t xmp_get_vol_fadeout(channel_t *chan, int8_t scale)
{
    if (!chan->keyon)
    {
        int32_t fo = chan->fadeout;
        if (fo)
        {
            fo -= chan->fadeout_step;
            if (fo < 0)
            {
                fo = 0;
                chan->mixer.volume = 0; // silence channel
            }
            chan->fadeout = fo;
        }
        return (int8_t)((uint32_t)(fo * scale) >> 16);
    }
    return scale;
}

// get song volume scale from master volume (passed in), song global volume, and song fade value
int8_t xmp_get_scale(song_t *song, int8_t master_vol)
{
    if (!master_vol)
        return 0;

    int32_t scale = song->fade;
    if (scale)
        scale = (uint16_t)(scale * song->volume) >> 6;
    else
        scale = song->volume;

    scale = (uint16_t)(scale * master_vol) >> 6;
    return scale;
}

// call from mixer at tempo rate
void xmp_update(song_t *song)
{
    if (!song)
        return;

    // early out if not playing
    if (!(song->state & XMP_PLAYING))
        return;

    // handle fade out
    if (song->fade)
    {
        song->fade--;
        if (!song->fade)
            song->state = XMP_STOPPED;
            return;
    }

    // update ticks and process if need be
    song->ticks++;
    if (song->ticks >= song->speed)
    {
        song->ticks = 0;
        song->current_row++;
        if (song->current_row >= song->pattern[song->current_pattern].rows)
        {
            song->current_row = 0;
            song->order_index++;
            if (song->order_index >= song->max_order)
            {
                song->state = XMP_STOPPED;
                if (song->loop)
                {
                    int32_t ch;

                    if (song->loop != XMP_LOOP_FOREVER)
                        song->loop--;

                    song->current_row = 0;
                    song->order_index = song->restart;
                    song->speed = song_table[song->mod]->speed;
                    song->tempo = song_table[song->mod]->tempo;
                    song->state = XMP_PLAYING;
                    song->current_pattern = song->order[song->order_index];
                    song->pattern_data = song->pattern[song->current_pattern].data;
                    song->ticks = song_table[song->mod]->speed; // force immediate processing

                    for (ch = 0; ch < song->channels; ch++)
                        memset(&song->channel[ch], 0, sizeof(channel_t));
                }
                return;
            }
            else
            {
                song->current_pattern = song->order[song->order_index];
                song->pattern_data = song->pattern[song->current_pattern].data;
            }
        }

        xm_row(song);
    }
    else
        xm_update(song); // update tick based effects
}
