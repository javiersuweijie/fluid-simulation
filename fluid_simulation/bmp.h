//
//  bmp.h
//  shader_test
//
//  Created by Javiersu on 6/4/15.
//  Copyright (c) 2015 Javiersu. All rights reserved.
//

#ifndef __shader_test__bmp__
#define __shader_test__bmp__

#include <stdio.h>
#include <stdlib.h>
#include <string>

struct BMPHeader
{
  char bfType[3];       /* "BM" */
  int bfSize;           /* Size of file in bytes */
  int bfReserved;       /* set to 0 */
  int bfOffBits;        /* Byte offset to actual bitmap data (= 54) */
  int biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
  int biWidth;          /* Width of image, in pixels */
  int biHeight;         /* Height of images, in pixels */
  short biPlanes;       /* Number of planes in target device (set to 1) */
  short biBitCount;     /* Bits per pixel (24 in this case) */
  int biCompression;    /* Type of compression (0 if no compression) */
  int biSizeImage;      /* Image size, in bytes (0 if no compression) */
  int biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
  int biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
  int biClrUsed;        /* Number of colors in the color table (if 0, use
                         maximum allowed by biBitCount) */
  int biClrImportant;   /* Number of important colors.  If 0, all colors
                         are important */
};

int SaveBMP(const char* filename, unsigned char* data, int width, int height);
#endif /* defined(__shader_test__bmp__) */
