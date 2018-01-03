#ifndef __MOUSE_H
#define __MOUSE_H

/** @defgroup mouse mouse
 * @{
 *
 * Function related with mouse
 */

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "bitmap.h"
#include "jumpingPeanut.h"

typedef struct {
  Bitmap* icon; // specifies the icon bitmap
  int x, y; // specifies x and y position of the player
  int delta_x, delta_y;  // specifies the x and y movement of the player

  int byte_being_read;  // specifies if a byte is being read
  unsigned long packet_array[3];  // specifies the array of packets.

  int left_button_d;  // specifies if the left button is down.
  int left_button_r;  // specifies if the left button is released

  int update;  // specifies if the Mouse is being update
  int draw;  // specifies x and y position of the player
} Peanut_mouse;

/**
 * @return a new struct Mouse.
 */
Peanut_mouse* new_mouse();

/**
 * @brief returns the game mouse struct if it exists, returns a new one if it
 * didn't exist
 *
 * @return struct Mouse
 */
Peanut_mouse* get_mouse();

/**
 * @brief Updates the mouse position given the last generated packet array.
 */
int update_mouse();

/**
 * @brief Draw the mouse.
 */
void draw_mouse();

/**
 * @brief Deletes the mouse.
 */
void delete_mouse();

/**
 * @brief Subscribes and enables mouse interrupts.
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int mouse_subscribe(void);

/**
 * @brief Unsubscribes mouse interrupts.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe(void);

/**
 * @brief Receives a command byte as an argument, allows to write in the mouse
 * and then does what the command is meant to do.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_cmd_maker(unsigned long cmd);

/**
 * @brief Disable the interrupts of the mouse.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int disable_mouse_interrupts();

/**
 * @brief Enable the interrupts of the mouse.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int enable_mouse_interrupts();

/**
 * @brief Read the byte.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned long mouse_handler();

/**
 * @brief Clear the outbuffer.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int clear_outbuf();

/** @} end of mouse */

#endif /* __MOUSE_H */
