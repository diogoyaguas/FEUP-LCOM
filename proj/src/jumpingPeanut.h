#ifndef _JUMPINGPEANUT_H
#define _JUMPINGPEANUT_H

/** @defgroup jumpingPeanut jumpingPeanut
 * @{
 *
 * Function related with the role-playing of the game
 */

#include "bitmap.h"
#include "stateMachine.h"
#include "rtc.h"
#include "cloud.h"

#define PLAYER_START_X 150
#define PLAYER_START_Y 384;
#define PLAYER_SPEED 3
#define GRAVITY 2
#define FPS 60  // Frames Per Second

typedef struct {
  int kbd_irq;    // specifies the keyboard irq
  int mouse_irq;  // specifies the mouse irq
  int timer_irq;  // specifies the timer irq
  int rtc_irq; // specifies the rtc irq

  Bitmap* background; // specifies the background bitmap
  Bitmap* menu; // specifies the menu bitmap
  Bitmap* gameOver; // specifies the gameover menu bitmap
  Bitmap* image_1; // specifies the image 1 of the player bitmap
  Bitmap* image_2; // specifies the image 2 of the player bitmap

  int y_player; // specifies the y position of the player
  int new_y_player; // specifies the new y position of the player
  int x_player; // specifies the x position of the player
  int jump_player; // specifies if the player is now jumping
  int score_player; // specifies the score of the player
  int life_player; // specifies the life of the player bitmap
  int collision_player; // specifies if the player collided with a cloud
  int collision_cloud; // specifies the cloud that the player collided
  int out_boundaries; // specifies if the player is out of boundaries

  Bitmap* zero; // specifies the zero bitmap
  Bitmap* one; // specifies the one bitmap
  Bitmap* two; // specifies the two bitmap
  Bitmap* three; // specifies the three bitmap
  Bitmap* four; // specifies the four bitmap
  Bitmap* five; // specifies the five bitmap
  Bitmap* six; // specifies the six bitmap
  Bitmap* seven; // specifies the seven bitmap
  Bitmap* eigth; // specifies the eigth bitmap
  Bitmap* nine; // specifies the nine bitmap
  Bitmap* points; // specifies the points bitmap

  Bitmap* heart; // specifies the heart bitmap

  Clouds* clouds[100]; // specifies the array of Clouds bitmap

  int done; // specifies if the game is done
  int draw; // specifies if the game is being draw
  unsigned long key; // specifies the key that was pressed
} JumpingPeanut;

/**
 * @brief Initializes the struct JumpingPeanut.
 *
 * @return the struct JumpingPeanut.
 */
JumpingPeanut* startJumpingPeanut();

/**
 * @brief Handles the interrupts from keyboard, mouse and timer.
 * @param Jumping Peanut
 * @param State machine
 * @param Time
 */
int updateJumpingPeanut(JumpingPeanut* peanut, State* state, Time* timer);

/**
 * @brief Draws all the game.
 * @param Jumping Peanut
 * @param Time
 */
void drawJumpingPeanut(JumpingPeanut* peanut, Time* timer);

/**
 * @brief Unscribes the keyboard, mouse and timer and deletes all the bitmaps.
 * @param Jumping Peanut
 */
void stopJumpingPeanut(JumpingPeanut* peanut);

/** @} end of jumpingPeanut */

#endif /* _JUMPINGPEANUT_H  */
