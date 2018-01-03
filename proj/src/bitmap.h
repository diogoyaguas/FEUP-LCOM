#ifndef BITMAP_H
#define BITMAP_H

/** @defgroup Bitmap Bitmap
* @{
*/

#include <stdio.h>

#define H_RES 1024
#define V_RES 768
#define MANGENTA_B1 0xF8
#define MANGENTA_B2 0X1F

typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } Alignment;

typedef struct {
  unsigned short type;    // specifies the file type
  unsigned int size;      // specifies the size in bytes of the bitmap file
  unsigned int reserved;  // reserved; must be 0
  unsigned int offset;    // specifies the offset in bytes from the
                          // bitmapfileheader to the bitmap bits
} BitmapFileHeader;

typedef struct {
  unsigned int size;  // specifies the number of bytes required by the struct
  int width;          // specifies width in pixels
  int height;         // specifies height in pixels
  unsigned short planes;     // specifies the number of color planes, must be 1
  unsigned short bits;       // specifies the number of bit per pixel
  unsigned int compression;  // specifies the type of compression
  unsigned int imageSize;    // size of image in bytes
  int xResolution;           // number of pixels per meter in x axis
  int yResolution;           // number of pixels per meter in y axis
  unsigned int nColors;      // number of colors used by the bitmap
  unsigned int importantColors;  // number of colors that are important
} BitmapInfoHeader;

/// Represents a Bitmap
typedef struct {
  BitmapInfoHeader bitmapInfoHeader;
  unsigned char* bitmapData;
} Bitmap;

/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws an unscaled, unrotated bitmap at the given position.
 *
 * @param Bitmap to be drawn
 * @param Destiny x coord
 * @param Destiny y coord
 * @param Image alignment
 */
void drawBitmap(Bitmap* bitmap, int x, int y, unsigned char* vm);

/**
 * @brief Draws a background bitmap at the given position.
 *
 * @param Bitmap to be drawn
 * @param Destiny x coord
 * @param Destiny y coord
 * @param Image alignment
 */
void drawBitmap_background(Bitmap* bmp, int x, int y, unsigned char* vm);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 * @param Bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

/** @} end of Bitmap */

#endif /* BITMAP_H */
