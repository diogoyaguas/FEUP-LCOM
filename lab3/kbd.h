#ifndef __KBD_H
#define __KBD_H

#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/drivers.h>
#include <minix/driver.h>

extern unsigned int counter_cmd;

int kbd_subscribe(void);

int kbd_unsubscribe(void);

int read_outbuf(unsigned long *byte);

int read_status_reg(unsigned long *byte);

void handler_c(void);

void write_code(unsigned long byte);

int read_outb_cmd(unsigned long cmd, unsigned long address);

int handler_c_poll(unsigned char *key_cmd);

int kbd_poll();

#endif /* __KBD_H */
