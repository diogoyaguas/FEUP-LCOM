#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H

#define VRAM_PHYS_ADDR	 0xF0000000
#define H_RES            1024
#define V_RES		         768
#define BITS_PER_PIXEL   8
#define BACKGROUND_COLOR 0

#include "sprite.h"

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 *
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 *
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
* @brief Set the pixel
*
* @return 0 upon success, non-zero upon failure
*/
unsigned long set_pixel(int x,int y);

/**
* @brief Gets the pixel position
*
* @return 0 upon success, non-zero upon failure
*/
unsigned long getPixelPosition(unsigned short x, unsigned short y);

/**
* @brief Draw a square by changing each pixel color
*
* @return 0 upon success, non-zero upon failure
*/
int draw_square(short x, short y, unsigned short size, unsigned long color);

/**
* @brief Draw a line by changing each pixel color
*
* @return 0 upon success, non-zero upon failure
*/
int draw_line(int xi, int yi, int xf, int yf, int color);

/**
* @brief Draw a sprite
*
* @return 0 upon success, non-zero upon failure
*/
void draw_sprite(Sprite *sp);

/**
* @brief Erases the sprite
*
* @return 0 upon success, non-zero upon failure
*/
void erase_sprite(Sprite *sp);

/**
* @brief Draw the xpm
*
* @return 0 upon success, non-zero upon failure
*/
int draw_xpm(char *xpm[], unsigned short xi, unsigned short yi);

 /** @} end of video_gr */

#endif /* __VIDEO_GR_H */
