#include <minix/drivers.h>
#include <math.h>
#include "rtc.h"
#include "i8042.h"
#include "i8254.h"
#include "timer.h"

static int hook_ID_RTC = 4;

Time* initTime() {
  Time* timer = (Time*)malloc(sizeof(Time));

  timer->hour = 0;
  timer->minute = 0;
  timer->second = 0;
  timer->day = 0;
  timer->month = 0;
  timer->year = 0;

  return timer;
}

int rtc_subscribe_int() {
  int hook_TEMP_RTC = hook_ID_RTC;

  if (sys_outb(RTC_ADDR_REG, REG_B) != OK) return -1;

  if (sys_outb(RTC_DATA_REG, RTC_CONF) != OK) return -1;

  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_ID_RTC) !=
      OK)
    return -1;

  if (sys_irqenable(&hook_ID_RTC) != OK) return -1;

  return BIT(hook_TEMP_RTC);
}

int rtc_unsubscribe_int() {
  if (sys_irqrmpolicy(&hook_ID_RTC) != OK) return -1;

  return 0;
}

void deleteTime(Time* time) { free(time); }

void getCurrentTime(Time* t) {
  unsigned long reg = 0;

  sys_outb(RTC_ADDR_REG, REG_C);

  unsigned long RegB = 0;

  sys_outb(RTC_ADDR_REG, REG_B);
  sys_inb(RTC_DATA_REG, &RegB);

  if (!(RegB & RTC_REGB_DM)) {
    sys_outb(RTC_ADDR_REG, SECONDS);
    sys_inb(RTC_DATA_REG, &reg);
    t->second = bcdToDec(reg);

    sys_outb(RTC_ADDR_REG, MINUTES);
    sys_inb(RTC_DATA_REG, &reg);
    t->minute = bcdToDec(reg);

    sys_outb(RTC_ADDR_REG, HOURS);
    sys_inb(RTC_DATA_REG, &reg);
    t->hour = bcdToDec(reg);

    sys_outb(RTC_ADDR_REG, YEAR);
    sys_inb(RTC_DATA_REG, &reg);
    t->year = bcdToDec(reg);

    sys_outb(RTC_ADDR_REG, MONTH);
    sys_inb(RTC_DATA_REG, &reg);
    t->month = bcdToDec(reg);

    sys_outb(RTC_ADDR_REG, DAY);
    sys_inb(RTC_DATA_REG, &reg);
    t->day = bcdToDec(reg);
  } else {
    sys_outb(RTC_ADDR_REG, SECONDS);
    sys_inb(RTC_DATA_REG, &reg);
    t->second = reg;

    sys_outb(RTC_ADDR_REG, MINUTES);
    sys_inb(RTC_DATA_REG, &reg);
    t->minute = reg;

    sys_outb(RTC_ADDR_REG, HOURS);
    sys_inb(RTC_DATA_REG, &reg);
    t->hour = reg;

    sys_outb(RTC_ADDR_REG, YEAR);
    sys_inb(RTC_DATA_REG, &reg);
    t->year = reg;

    sys_outb(RTC_ADDR_REG, MONTH);
    sys_inb(RTC_DATA_REG, &reg);
    t->month = reg;

    sys_outb(RTC_ADDR_REG, DAY);
    sys_inb(RTC_DATA_REG, &reg);
    t->day = reg;
  }
}

unsigned long bcdToDec(unsigned long bcd) {
  return (((bcd & 0xF0) >> 4) * 10) + (bcd & 0x0F);
}
