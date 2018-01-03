#ifndef __KEYBOARD_H
#define __KEYBOARD_H

/** @defgroup keyboard keyboard
 * @{
 *
 * Function related with keyboard
 */

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>

unsigned long key;

/**
 * @brief Subscribes and enables keyboard interrupts.
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int kbd_subscribe(void);

/**
 * @brief Unsubscribes keyboard interrupts.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int kbd_unsubscribe(void);

/**
 * @brief Read the scancode from the OUT_BUF.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int read_outbuf(unsigned long *byte);

/**
 * @brief Read the status.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int read_status_reg(unsigned long *byte);

/**
 * @brief Handler.
 *
 * @return Return 0 upon success and non-zero otherwise
 */
void handler_c(void);

/** @} end of keyboard */

#endif /* __KEYBOARD_H */
