#include "mouse.h"
#include "i8042.h"
#include "i8254.h"
#include "video_gr.h"
#include "jumpingPeanut.h"

int hook_ID_MOUSE = 2;

Peanut_mouse* mouse = NULL;
Peanut_mouse state_mouse;

Peanut_mouse* get_mouse() {
  if (!mouse) {
    enable_mouse_interrupts();
    mouse = new_mouse();
  }

  return mouse;
}

Peanut_mouse* new_mouse() {
  Peanut_mouse* mouse = (Peanut_mouse*)malloc(sizeof(Peanut_mouse));

  mouse->icon = loadBitmap("/home/lcom/proj/res/mouse.bmp");
  mouse->x = H_RES / 2;
  mouse->y = V_RES / 2;
  mouse->draw = 1;

  return mouse;
}

int update_mouse() {
  static unsigned int packet_number = 0;
  static unsigned int packet_read = 0;

  unsigned long byte = mouse_handler();

  if (byte == -1) return -1;

  switch (packet_number) {
    case 0:
      if (byte & BIT(3)) {
        mouse->packet_array[packet_number] = byte;
        packet_number++;
      }
      packet_read = 0;
      break;

    case 1:
      mouse->packet_array[packet_number] = byte;
      packet_number++;
      packet_read = 0;
      break;
    case 2:
      mouse->packet_array[packet_number] = byte;
      packet_number = 0;
      packet_read = 1;
      break;
  }

  if (packet_read) {
    mouse->delta_x = (unsigned char)mouse->packet_array[1];
    mouse->delta_y = (unsigned char)mouse->packet_array[2];

    if ((X_SIGN & mouse->packet_array[0]) == X_SIGN) {
      mouse->delta_x -= 256;
    }
    if ((Y_SIGN & mouse->packet_array[0]) == Y_SIGN) {
      mouse->delta_y -= 256;
    }

    if (mouse->x + mouse->delta_x + 30 >= H_RES)
      mouse->x = H_RES - 30;
    else
      mouse->x += mouse->delta_x;

    if (mouse->y + mouse->delta_y + 38 >= V_RES)
      mouse->y = V_RES - 38;
    else
      mouse->y -= mouse->delta_y;

    if (mouse->x + mouse->delta_x <= 0)
      mouse->x = 0;
    else
      mouse->x += mouse->delta_x;

    if (mouse->y - mouse->delta_y <= 0)
      mouse->y = 0;
    else
      mouse->y -= mouse->delta_y;

    if (mouse->packet_array[0] & MOUSE_LB) {
      mouse->left_button_d = 1;
      mouse->left_button_r = 0;
    } else if (mouse->left_button_d) {
      mouse->left_button_d = 0;
      mouse->left_button_r = 1;
    } else {
      mouse->left_button_d = 0;
      mouse->left_button_r = 0;
    }
  }

  return packet_read;
}

void draw_mouse() { bitmapDrawer(mouse->icon, mouse->x, mouse->y); }

void delete_mouse() { free(get_mouse()); }

int mouse_subscribe(void) {
  
  int hook_TEMP_MOUSE = hook_ID_MOUSE;

  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE,
                       &hook_ID_MOUSE) != OK) {
    return -1;
  }

  if (sys_irqenable(&hook_ID_MOUSE) != OK) {
    return -1;
  }
  return BIT(hook_TEMP_MOUSE);
}

int mouse_unsubscribe(void) {
  if (sys_irqdisable(&hook_ID_MOUSE) != OK) {
    return -1;
  }

  if (sys_irqrmpolicy(&hook_ID_MOUSE) != OK) {
    return -1;
  }

  return 0;
}

int mouse_cmd_maker(unsigned long cmd) {
  unsigned long status, byte;

  do {
    if (sys_inb(STAT_REG, &status) != OK)
      return -1;  // verify the status of the buffer

    if ((status & IBF) == 0) {
      if (sys_outb(CMD_BUF, WRT_BYTE_MOUSE) != OK)
        return -1;  // allow to write in the mouse

      if (sys_outb(OUT_BUF, cmd) != OK) return -1;  // enable to transmit data

      tickdelay(micros_to_ticks(DELAY_US));

      if (sys_inb(OUT_BUF, &byte) != OK) return -1;
    }

  } while (byte != MOUSE_ACK);

  return 0;
}

int disable_mouse_interrupts() {
  unsigned long status, byte;

  if (sys_inb(STAT_REG, &status) != OK)
    return -1;  // verify the status of the buffer

  if ((status & IBF) == 0) {
    if (sys_outb(CMD_BUF, READ_CMD_BYTE) != OK)
      return -1;  // ask permission to read the command byte

    if (sys_inb(OUT_BUF, &byte) != OK) return -1;  // read the command byte

    byte = byte & BIT1_0;  // sets BIT(1) to disable mouse interrupts
  }

  if (sys_outb(CMD_BUF, WRITE_CMD_BYTE) != OK)
    return -1;  // ask permission to write

  if (sys_outb(IN_BUF, byte) != OK) return -1;  // write the new command byte

  return 0;
}

int enable_mouse_interrupts() {
  unsigned long byte, status;

  if (sys_inb(STAT_REG, &status) != OK)
    return -1;  // verify the status of the buffer

  if ((status & IBF) == 0) {
    if (sys_outb(CMD_BUF, READ_CMD_BYTE) != OK)
      return -1;  // ask permission to read the command byte

    if (sys_inb(OUT_BUF, &byte) != OK) return -1;  // read the command byte

    byte = byte | BIT(1);  // sets BIT(1) to enable mouse interrupts
  }

  if (sys_outb(CMD_BUF, WRITE_CMD_BYTE) != OK)
    return -1;  // ask permission to write

  if (sys_outb(IN_BUF, byte) != OK) return -1;  // write the new command byte

  return 0;
}

unsigned long mouse_handler() {
  unsigned long byte;

  if (sys_inb(OUT_BUF, &byte) != OK) return -1;

  return byte;
}

int clear_outbuf() {
  unsigned long status;

  if (sys_inb(STAT_REG, &status) != OK) return -1;

  while ((status & BIT(0)) != 0) {
    if (sys_inb(OUT_BUF, &status) != OK) return -1;

    tickdelay(micros_to_ticks(DELAY_US));

    if (sys_inb(STAT_REG, &status) != OK) return -1;
  }

  return 0;
}
