#include <string.h>
#include <xmp.h>

#include "32x.h"
#include "sound.h"

int16_t __attribute__((aligned(16))) snd_buffer[MAX_NUM_SAMPLES*2*2];

uint16_t num_samples = 441;

char temp[80];

static mixer_t __attribute__((aligned(16))) sfx_mixers[NUM_SFX_MIXERS];

extern int8_t mvolume;

extern song_t *song;

static inline void flush_mem(void *ptr, int32_t len)
{
    while (len > 0)
    {
        CacheClearLine(ptr);
        ptr += 16;
        len -= 16;
    }
}

static void do_sfx(int16_t *buffer)
{
    uint8_t i;

    for (i=0; i<NUM_SFX_MIXERS; i++)
        if (sfx_mixers[i].data)
            MixSamples(&sfx_mixers[i], buffer, num_samples, 64);
}

static void do_music(int16_t *buffer)
{
    int32_t scale;
    uint16_t bpm;
    uint8_t nch, i;
    static song_t *last_song = (song_t *)NULL;
    song_t **usong = (song_t **)((int32_t)&song | 0x20000000);
    song_t *lsong;
    int8_t *umvolume = (int8_t *)((int32_t)&mvolume | 0x20000000);

    lsong = (song_t *)*usong; // get local song pointer from uncached song handle
    if (((int32_t)lsong & 0x1FFFFFFF) == 0)
    {
        last_song = (song_t *)NULL;
        return;
    }

    if (lsong != last_song)
    {
        // song changed - flush entire struct
        flush_mem(lsong, sizeof(song_t) - sizeof(channel_t)); // flush song variables
        nch = xmp_num_channels(lsong);
        for (i = 0; i < nch; i++)
            flush_mem(xmp_get_channel(lsong, i), sizeof(channel_t));
        last_song = lsong;
    }
    else
        CacheClearLine(lsong); // just clear shared state variables

    if (!xmp_is_playing(lsong) || xmp_is_paused(lsong))
        return;

    // process music
    xmp_update(lsong);
    if (!xmp_is_playing(lsong))
        return; // just stopped

    bpm = xmp_get_tempo(lsong);
    num_samples = (SAMPLE_RATE * 5)/(bpm * 2);
    if (num_samples > MAX_NUM_SAMPLES)
        num_samples = MAX_NUM_SAMPLES;

    // mix music samples
    scale = xmp_get_scale(song, *umvolume);
    if (scale)
    {
        nch = xmp_num_channels(lsong);
        for (i = 0; i < nch; i++)
        {
            channel_t *chan = xmp_get_channel(lsong, i);
            mixer_t *mix = (mixer_t *)chan;
            // process envelope
            xmp_proc_vol_envelope(chan);
            // mix channel to sound buffer
            if (mix->data && mix->volume)
            {
                int32_t env = xmp_get_vol_envelope(chan);
                int8_t cscale = xmp_get_vol_fadeout(chan, scale);
                if (env == -1)
                {
                    // no envelope - just use scale
                    MixSamples(mix, buffer, num_samples, cscale);
                }
                else
                {
                    // envelope enabled - use envelope value * scale / 64
                    MixSamples(mix, buffer, num_samples, (int8_t)((uint16_t)(cscale * env) >> 6));
                }
            }
        }
    }
}

/*
 * functions called from main
 */

void fill_buffer(int16_t *buffer)
{
    int16_t i;

    memset(buffer, 0, num_samples * 4);

    LockMixer(MIXER_LOCK_SSH2);
    do_music(buffer);
    do_sfx(buffer);
    UnlockMixer();

    // convert buffer from s16 pcm samples to u16 pwm samples
    for (i = 0; i < num_samples*2; i++)
    {
        int16_t s = *buffer + SAMPLE_CENTER;
        *buffer++ = (s < 0) ? SAMPLE_MIN : (s > SAMPLE_MAX) ? SAMPLE_MAX : s;
    }
}

uint8_t start_sfx(int8_t *data, uint16_t srate, uint32_t loop_length, uint32_t length, uint8_t volume, uint8_t pan)
{
    uint8_t i;

    LockMixer(MIXER_LOCK_MSH2);

    // look for free mixer
    for (i=0; i<NUM_SFX_MIXERS; i++)
    {
        CacheClearLine(&sfx_mixers[i].data);
        if (!sfx_mixers[i].data)
        {
            // found free mixer
            sfx_mixers[i].data = data;
            sfx_mixers[i].position = 0;
            sfx_mixers[i].increment = (srate << 14) / SAMPLE_RATE;
            sfx_mixers[i].length = length << 14;
            sfx_mixers[i].loop_length = loop_length << 14;
            sfx_mixers[i].volume = volume;
            sfx_mixers[i].pan = pan;
            UnlockMixer();
            return i;
        }
    }

    // look for same sfx
    for (i=0; i<NUM_SFX_MIXERS; i++)
    {
        if (sfx_mixers[i].data == data)
        {
            // found same sfx - restart
            sfx_mixers[i].position = 0;
            sfx_mixers[i].increment = (srate << 14) / SAMPLE_RATE;
            sfx_mixers[i].length = length << 14;
            sfx_mixers[i].loop_length = loop_length << 14;
            sfx_mixers[i].volume = volume;
            sfx_mixers[i].pan = pan;
            UnlockMixer();
            return i;
        }
    }

    UnlockMixer();
    return 0xFF; // failed
}

void update_sfx(uint8_t mix, int32_t srate, int32_t volume, int32_t pan)
{
    LockMixer(MIXER_LOCK_MSH2);

    CacheClearLine(&sfx_mixers[mix].data);
    if (sfx_mixers[mix].data)
    {
        // still playing - update parameters
        if (srate != -1)
            sfx_mixers[mix].increment = (srate << 14) / SAMPLE_RATE;
        if (volume != -1)
            sfx_mixers[mix].volume = volume;
        if (volume != -1)
            sfx_mixers[mix].pan = pan;
    }

    UnlockMixer();
}

void stop_sfx(uint8_t mix)
{
    LockMixer(MIXER_LOCK_MSH2);

    sfx_mixers[mix].data = 0;

    UnlockMixer();
}

uint8_t check_sfx(uint8_t mix)
{
    uint8_t res = 0;

    LockMixer(MIXER_LOCK_MSH2);

    CacheClearLine(&sfx_mixers[mix].data);
    if (sfx_mixers[mix].data)
        res = 1;

    UnlockMixer();
    return res;
}
