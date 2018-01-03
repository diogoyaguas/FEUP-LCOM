#include <minix/drivers.h>
#include "jumpingPeanut.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "game.h"
#include "i8042.h"
#include "i8254.h"
#include "stateMachine.h"
#include "cloud.h"

Clouds* startClouds() {
  Clouds* clouds = (Clouds*)malloc(sizeof(Clouds));

  clouds->cloud_1 = loadBitmap("/home/lcom/proj/res/cloud1.bmp");
  clouds->cloud_2 = loadBitmap("/home/lcom/proj/res/cloud2.bmp");
  clouds->cloud_3 = loadBitmap("/home/lcom/proj/res/cloud3.bmp");
  clouds->cloud_4 = loadBitmap("/home/lcom/proj/res/cloud1.bmp");
  clouds->cloud_5 = loadBitmap("/home/lcom/proj/res/cloud2.bmp");

  clouds->cloud1_y = CLOUD1_Y;
  clouds->cloud2_y = CLOUD2_Y;
  clouds->cloud3_y = CLOUD3_Y;
  clouds->cloud4_y = CLOUD4_Y;
  clouds->cloud5_y = CLOUD5_Y;
  clouds->width = CLOUD_WIDTH;

  clouds->clouds_x = CLOUDS_INIT_X;

  clouds->clouds_speed = CLOUDS_SPEED;

  return clouds;
}

void drawClouds(Clouds* clouds) {
  switch (clouds->type) {
    case 0:

      bitmapDrawer(clouds->cloud_2, clouds->clouds_x, clouds->cloud2_y);
      bitmapDrawer(clouds->cloud_3, clouds->clouds_x, clouds->cloud3_y);
      bitmapDrawer(clouds->cloud_4, clouds->clouds_x, clouds->cloud4_y);
      bitmapDrawer(clouds->cloud_5, clouds->clouds_x, clouds->cloud5_y);

      break;

    case 1:

      bitmapDrawer(clouds->cloud_1, clouds->clouds_x, clouds->cloud1_y);
      bitmapDrawer(clouds->cloud_3, clouds->clouds_x, clouds->cloud3_y);
      bitmapDrawer(clouds->cloud_4, clouds->clouds_x, clouds->cloud4_y);
      bitmapDrawer(clouds->cloud_5, clouds->clouds_x, clouds->cloud5_y);

      break;

    case 2:

      bitmapDrawer(clouds->cloud_1, clouds->clouds_x, clouds->cloud1_y);
      bitmapDrawer(clouds->cloud_2, clouds->clouds_x, clouds->cloud2_y);
      bitmapDrawer(clouds->cloud_4, clouds->clouds_x, clouds->cloud4_y);
      bitmapDrawer(clouds->cloud_5, clouds->clouds_x, clouds->cloud5_y);

      break;

    case 3:

      bitmapDrawer(clouds->cloud_1, clouds->clouds_x, clouds->cloud1_y);
      bitmapDrawer(clouds->cloud_2, clouds->clouds_x, clouds->cloud2_y);
      bitmapDrawer(clouds->cloud_3, clouds->clouds_x, clouds->cloud3_y);
      bitmapDrawer(clouds->cloud_5, clouds->clouds_x, clouds->cloud5_y);

      break;

    case 4:

      bitmapDrawer(clouds->cloud_1, clouds->clouds_x, clouds->cloud1_y);
      bitmapDrawer(clouds->cloud_2, clouds->clouds_x, clouds->cloud2_y);
      bitmapDrawer(clouds->cloud_3, clouds->clouds_x, clouds->cloud3_y);
      bitmapDrawer(clouds->cloud_4, clouds->clouds_x, clouds->cloud4_y);

      break;
  }
}

void deleteClouds(Clouds* clouds) { free(clouds); }
