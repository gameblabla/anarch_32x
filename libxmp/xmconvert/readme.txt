XM Tracker Conversion Utility - xmconvert
v1.0 by Richard Quirk
v1.1 by Chilly Willy, 2012-09-15
v1.2 by Chilly Willy, 2012-09-16

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version. License details can be found in the COPYING file.

To compile: Change to the xmconvert directory, run configure, then make.

cd xmconvert
./configure
make

To use: Run "xmconvert {OPTIONS} file-list" to convert all the files in
the list to the form needed by libxmp. I suggest you run it from the directory
where you want the compiled data files to be, and use paths to xmconvert and
each of the xm files, like this:

cd music-files
../tools/xmconvert -v -s -e ../Doom/D_Intro.xm ../Doom/D_Inter.xm

The options may be one or more of the following:

-v  Print out more info while converting the files
-u  Output the samples as unsigned data (default is signed data)
-s  Add an underscore to labels in the assembler files
-e  Include the volume envelope with the samples
-h  Print the help message and exit without doing anything

Certain CPUs need underscores before the labels in the assembler files. The
SH2 is one example. The ARM does not.

The volume envelope is needed for some instruments. You can't really tell
ahead of time - you just need to try it without envelopes and see if there
is a problem. I've seen sound effect and percussion patches that needed the
volume envelope, but most do not.

Whether you need signed or unsigned sample data depends on the platform and
mixer code. For example, using signed data makes it easy to mix. However,
if you had a platform with many hardware sound channels that used unsigned
samples, you would want unsigned sample data instead.


If you have any problems, contact Chilly Willy at <chillywillyguru@gmail.com>

The original xmconvert was by Richard Quirk <richard.quirk@gmail.com>
