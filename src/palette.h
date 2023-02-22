/*
  @file palette.h

  General purpose HSV-based 256 color palette.

  by Miloslav Ciz (drummyfish), 2019

  Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
  plus a waiver of all other intellectual property. The goal of this work is
  be and remain completely in the public domain forever, available for any use
  whatsoever.
*/

#ifndef PALETTE_256_H
#define PALETTE_256_H

SFG_PROGRAM_MEMORY


uint16_t paletteRGB565[256] = {
0, 6371, 14855, 27436, 38066, 48631, 59228, 65535, 6241, 14563, 24966, 33320, 
43755, 52142, 62577, 64885, 6337, 16772, 25190, 35689, 44139, 52559, 63058, 
65333, 6402, 14851, 23334, 31816, 40268, 50830, 59314, 65526, 4354, 10755, 
15174, 21576, 30027, 36462, 42929, 51190, 2306, 8709, 13096, 19532, 25935, 
30323, 36790, 47098, 4356, 8711, 15115, 19536, 27956, 32377, 38846, 47103, 
4227, 8519, 12812, 17136, 25588, 31961, 38334, 46751, 2115, 8424, 14732, 21040, 
27380, 33721, 40030, 48511, 6244, 12520, 22924, 31280, 39669, 48089, 56445, 
64927, 8290, 16614, 24969, 33325, 43761, 52148, 62585, 64892, 10240, 18464, 
26657, 38946, 47202, 55524, 63781, 64074, 10400, 18753, 27170, 37601, 48034, 
56421, 64837, 65002, 6496, 14944, 23425, 31937, 40418, 48869, 57317, 61418, 
352, 4704, 7041, 7362, 15842, 20196, 24550, 30697, 354, 611, 2949, 5288, 7658, 
12013, 8175, 20467, 357, 617, 910, 5298, 9686, 7931, 14335, 24575, 69, 233, 
4461, 4594, 8918, 2907, 13311, 19679, 4133, 2089, 4173, 8306, 10455, 16667, 
20863, 27263, 6149, 14377, 22574, 28819, 39063, 45371, 53631, 57983, 10242, 
18469, 26664, 38987, 47247, 55537, 63797, 64119, 10272, 18432, 26624, 34848, 
45056, 53312, 61504, 63520, 10336, 18624, 26976, 35296, 45728, 54048, 62400, 
64609, 8544, 16992, 25440, 31872, 42400, 50880, 59328, 65504, 4448, 2656, 
7008, 11392, 13728, 14016, 20416, 26593, 2400, 608, 864, 3200, 5504, 1698, 
1985, 2019, 353, 614, 872, 1163, 1422, 1713, 2005, 2039, 197, 361, 557, 753, 
950, 5273, 1406, 9759, 5, 9, 2093, 81, 4214, 186, 2270, 351, 2052, 4105, 4109, 
8209, 8278, 12314, 16414, 18527, 10245, 14378, 22541, 30738, 38934, 47130, 55326, 
61471, 10241, 18435, 26630, 34824, 45066, 53293, 61519, 63601
};

/** Adds value (brightness), possibly negative, to given color (represented by
  its palette index). If you know you'll only be either adding or substracting,
  use plusValue() or minusValue() functions, which should be faster. */
static inline uint8_t palette_addValue(uint8_t color, int8_t add)
{
  uint8_t newValue = color + add;
  
  if ((newValue >> 3) == (color >> 3))
    return newValue;
  else
    return add > 0 ? (color | 0x07) : 0;
}

/** Adds a positive value (brightness) to given color (represented by its
  palette index). This should be a little bit faster than addValue(). */
static inline uint8_t palette_plusValue(uint8_t color, uint8_t plus)
{
  uint8_t newValue = color + plus;
  return ((newValue >> 3) == (color >> 3)) ? newValue : (color | 0x07);
}

/** Substracts a positive value (brightness) from given color (represented by
  its palette index). This should be a little bit faster than addValue(). */
static inline uint8_t palette_minusValue(uint8_t color, uint8_t minus)
{
  uint8_t newValue = color - minus;
  return ((newValue >> 3) == (color >> 3)) ? newValue : 0;
}

#endif //guard
