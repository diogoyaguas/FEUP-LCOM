#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "jumpingPeanut.h"

int hook_ID_KBD = 1;

int kbd_subscribe(void) {
  int hook_TEMP_KBD = hook_ID_KBD;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_ID_KBD) !=
      OK) {
    return -1;
  }

  if (sys_irqenable(&hook_ID_KBD) != OK) {
    return -1;
  }

  return BIT(hook_TEMP_KBD);
}

int kbd_unsubscribe(void) {
  if (sys_irqdisable(&hook_ID_KBD) != OK) {
    return 1;
  }

  if (sys_irqrmpolicy(&hook_ID_KBD) != OK) {
    return 1;
  }

  return 0;
}

int read_outbuf(unsigned long *byte) {
  unsigned long status;

  while (1) {
    if (read_status_reg(&status) == -1) return -1;

    if ((status & BIT(0)) == 1) {
      if (sys_inb(OUT_BUF, byte) != OK) return -1;
      return 0;
    } else
      return -1;

    tickdelay(micros_to_ticks(DELAY_US));
  }

  return 0;
}

int read_status_reg(unsigned long *byte) {
  if (sys_inb(STATUS_PORT, byte) != OK) return -1;

  return 0;
}

void handler_c() {
  unsigned long byte;

  if (read_outbuf(&byte) == -1) {
    return;

  } else {
    key = byte;
  }
}
