#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "i8254.h"
#include "timer.h"

int hook_ID = 0;

int timer_subscribe_int(void) {
  int hook_TEMP = hook_ID;

  if (sys_irqsetpolicy(TIMER_IRQ, IRQ_REENABLE, &hook_ID) != OK) {
    return -1;
  }

  if (sys_irqenable(&hook_ID) != OK) {
    return -1;
  }

  return BIT(hook_TEMP);
}

int timer_unsubscribe_int() {
  if (sys_irqdisable(&hook_ID) != OK) {
    return 1;
  }

  if (sys_irqrmpolicy(&hook_ID) != OK) {
    return 1;
  }

  return 0;
}
