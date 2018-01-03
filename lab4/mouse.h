#ifndef __MOUSE_H
#define __MOUSE_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>

extern unsigned int counter_bytes;

extern unsigned long packet_array[3];

typedef enum {INIT, DRAW, COMP} state_t;
typedef enum {RDOWN, RUP, MOVE} ev_type_t;

/**
 * @brief Subscribes and enables mouse interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int mouse_subscribe(void);

/**
 * @brief Unsubscribes mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe(void);

/**
 * @brief Receives a command byte as an argument, allows to write in the mouse and then does what the command is meant to do
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_cmd_maker(unsigned long cmd);

/**
 * @brief Waits while the outbuffer is read
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int wait_output_buffer(long ms);

/**
 * @brief Disable the interrupts of the mouse
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int disable_mouse_interrupts();

/**
 * @brief Enable the interrupts of the mouse
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int enable_mouse_interrupts();

/**
 * @brief Read the byte
 *
 * @return Return 0 upon success and non-zero otherwise
 */
unsigned long mouse_handler();

/**
 * @brief Print the packet
 */
void print_packet();

/**
 * @brief Clear the outbuffer
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int clear_outbuf();

/**
 * @brief Exit condiction when the user makes a continues movement with a positive slope in the same direction
 *
 */
void check_hor_line(ev_type_t evt, long *x, long *y, short *length, short *total, state_t *st);

#endif
