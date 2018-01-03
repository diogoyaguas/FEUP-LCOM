#ifndef _CLOUD_H
#define _CLOUD_H

/** @defgroup cloud cloud
 * @{
 *
 * Function of the Clouds
 */

#include "bitmap.h"

#define CLOUD_WIDTH 260
#define CLOUD1_Y 0
#define CLOUD2_Y 150
#define CLOUD3_Y 300
#define CLOUD4_Y 450
#define CLOUD5_Y 600
#define CLOUDS_INIT_X H_RES
#define CLOUDS_SPEED 2

typedef struct {
  Bitmap* cloud_1;
  Bitmap* cloud_2;
  Bitmap* cloud_3;
  Bitmap* cloud_4;
  Bitmap* cloud_5;

  int cloud1_y;
  int cloud2_y;
  int cloud3_y;
  int cloud4_y;
  int cloud5_y;
  int clouds_x;
  int width;
  int clouds_speed;
  int type;

} Clouds;

/**
 * @brief Initializes what is related to the sturct Clouds.
 */
Clouds* startClouds();

/**
 * @brief Draws the set of clouds.
 * @param Cloud to be draw
 */
void drawClouds(Clouds* clouds);

/**
 * @brief Deletes the clouds bitmaps.
 * @param Cloud to be delete
 */
void deleteClouds(Clouds* clouds);

/** @} end of cloud */

#endif /* _CLOUD_H  */
