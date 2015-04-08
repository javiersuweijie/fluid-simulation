//
//  bmp.cpp
//  shader_test
//
//  Created by Javiersu on 6/4/15.
//  Copyright (c) 2015 Javiersu. All rights reserved.
//

#include "bmp.h"

int SaveBMP(const char *filename, unsigned char* data, int width, int height)
{
  int i, j, ipos;
  int bytesPerLine;
  unsigned char *line;
  FILE *file;
  struct BMPHeader bmph;
  
  /* The length of each line must be a multiple of 4 bytes */
  
  bytesPerLine = (3 * (width + 1) / 4) * 4;
  
  std::strcpy(bmph.bfType, "BM");
  bmph.bfOffBits = 54;
  bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
  bmph.bfReserved = 0;
  bmph.biSize = 40;
  bmph.biWidth = width;
  bmph.biHeight = height;
  bmph.biPlanes = 1;
  bmph.biBitCount = 24;
  bmph.biCompression = 0;
  bmph.biSizeImage = bytesPerLine * height;
  bmph.biXPelsPerMeter = 0;
  bmph.biYPelsPerMeter = 0;
  bmph.biClrUsed = 0;
  bmph.biClrImportant = 0;
  
  file = fopen (filename, "wb");
  if (file == NULL) return(0);
  
  fwrite(&bmph.bfType, 2, 1, file);
  fwrite(&bmph.bfSize, 4, 1, file);
  fwrite(&bmph.bfReserved, 4, 1, file);
  fwrite(&bmph.bfOffBits, 4, 1, file);
  fwrite(&bmph.biSize, 4, 1, file);
  fwrite(&bmph.biWidth, 4, 1, file);
  fwrite(&bmph.biHeight, 4, 1, file);
  fwrite(&bmph.biPlanes, 2, 1, file);
  fwrite(&bmph.biBitCount, 2, 1, file);
  fwrite(&bmph.biCompression, 4, 1, file);
  fwrite(&bmph.biSizeImage, 4, 1, file);
  fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
  fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
  fwrite(&bmph.biClrUsed, 4, 1, file);
  fwrite(&bmph.biClrImportant, 4, 1, file);
  
  line = (unsigned char *)malloc(bytesPerLine);
  if (line == NULL)
  {
    fprintf(stderr, "Can't allocate memory for BMP file.\n");
    return(0);
  }
  
  for (i = 0; i < height ; i++)
  {
    for (j = 0; j < width; j++)
    {
      ipos = (width * i + j)*3;
      line[3*j]  = data[ipos+2];
      line[3*j+1] = data[ipos+1];
      line[3*j+2] = data[ipos];
    }
    fwrite(line, bytesPerLine, 1, file);
  }
  
  free(line);
  fclose(file);
  
  return(1);
}