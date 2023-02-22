XM Player library - libxmp
v1.0 by Chilly Willy, 2012-09-15, based partly on libsxmp by Richard Quirk.
v1.1 by Chilly Willy, 2012-09-16

License details can be found in the COPYING file included with the library.

To use: XM files must first be converted using xmconvert, included with the
library. This generates source files to include in your project. You then
pass an index to the song to play to xmp_start_song(). The result is a pointer
to a song structure with all the info libxmp needs to play the song. More
than one song can be started, allowing for nested music. Once a song has been
started, the audio mixer must update the song in the mixing loop once every
beat by calling xmp_update(song_pointer). If the instrument volume envelopes
are used, the mixer should call xmp_proc_vol_envelope() on each channel, and
fetch the envelope value using xmp_get_vol_envelope() to use in computing
the scale for the channel volume in mixing. Once you are done with a song,
you should call xmp_stop_song(song_pointer) and then free the song structure.
See the example XMPlayer for exact details on all this.

Known limitations: The biggest limitation is the player only uses linear
frequencies. Amiga frequencies will sound "funny" when played on libxmp.
Panning envelopes for instruments are not supported. Only the first sample
from each instrument is used. Standard effects not (yet) supported include
EFFECT_GLISSANDO_CONTROL, EFFECT_PATTERN_LOOP, EFFECT_PATTERN_DELAY, and
EFFECT_INVERT_LOOP. Extended effects not (yet) supported include
EFFECT_SET_ENVELOPE_POS, EFFECT_MULTI_RETRIG_NOTE, and EFFECT_TREMOR.

The library has the ability to use a custom allocation function called
xmp_malloc(). If you wish to use the standard posix function, malloc(),
you must define USE_MALLOC. If you wish to use instrument volume envelopes,
you must define USE_VOL_ENVELOPE. The library does not itself handle any
cache issues. See the 32X libxmp version and 32X XMPlayer for examples of
dealing with SMP and cache issues.

The library is designed to be portable, using only standard C. Everything
is designed so that only the function to mix samples into the sound buffer
need be assembly, and then only on platforms with slow CPUs.

Function details:
------------------------------------------------------------------------

song_t *xmp_start_song(uint8_t mod, uint8_t loop);

Allocate and initialize a song structure for the module specified by the
index, mod. The loop argument determines if the song will be played once
(0), N times (1 to 254), or forever (255). The return value is a pointer
to the song structure, or NULL if something went wrong.

------------------------------------------------------------------------

void xmp_stop_song(song_t *song);

Stops the song passed in. The song structure can then be freed using a
free(song) command.

------------------------------------------------------------------------

uint8_t xmp_is_playing(song_t *song);

Returns 0 is the song passed in is not playing, or 1 if it is playing.

------------------------------------------------------------------------

void xmp_pause(song_t *song, uint8_t pause);

Pass pause of 1 to pause the song passed in. Pass pause of 0 to resume
playing.

------------------------------------------------------------------------

uint8_t xmp_is_paused(song_t *song);

If the song passed in is paused, the function returns 1, otherwise, it
returns 0.

------------------------------------------------------------------------

void xmp_set_fade(song_t *song, int8_t fade);

Start the song fading out. Once it is done fading, it automatically stops.
The fade argument determines how long it will take to fade (maximum value
is 64).

------------------------------------------------------------------------

uint8_t xmp_get_tempo(song_t *song);

The beats per minute of the song passed in are returned.

------------------------------------------------------------------------

uint8_t xmp_num_channels(song_t *song);

The number of channels in the song passed in are returned.

------------------------------------------------------------------------

channel_t *xmp_get_channel(song_t *song, uint8_t chan);

A pointer to the channel indicated by chan in the song passed in is returned.

------------------------------------------------------------------------

void xmp_proc_vol_envelope(channel_t *chan);

If the current instrument playing on the channel passed in has the volume
envelope enabled, the envelope is processed.

------------------------------------------------------------------------

int8_t xmp_get_vol_envelope(channel_t *chan);

If the current instrument playing on the channel passed in has the volume
envelope enabled, the current envelope value is returned. The maximum value
is 64. Otherwise, -1 is returned and should be checked for.

------------------------------------------------------------------------

int8_t xmp_get_vol_fadeout(channel_t *chan, int8_t scale);

If the current instrument playing on the channel is in key-off, subtract
the instrument fadeout step and scale the volume scale, otherwise, return
the scale unchanged.

------------------------------------------------------------------------

int8_t xmp_get_scale(song_t *song, int8_t master_vol);

The volume scale for the song passed in using the master volume passed in
is returned. It incorporates the global song volume in this value. The
maximum value is 64. The maximum value for master volume you can pass is
XMP_MAX_VOLUME (64).

------------------------------------------------------------------------

void xmp_update(song_t *song);

This function must be called at the song tempo to update the song state.

------------------------------------------------------------------------

If you have any problems, contact Chilly Willy at <chillywillyguru@gmail.com>

The original libsxmp was by Richard Quirk <richard.quirk@gmail.com>
