#!/bin/sh
for i in *.raw; do ffmpeg -f u8 -y -ar 8000 -ac 1 -i "$i" -f s8 -acodec pcm_s8 -ar 22050 "$i.32x"; done
