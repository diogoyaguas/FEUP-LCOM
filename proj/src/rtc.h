#ifndef __RTC_H
#define __RTC_H

/** @defgroup rtc rtc
 * @{
 *
 * Function related with RTC
 */

typedef struct {
  unsigned long hour;
  unsigned long minute;
  unsigned long second;

  unsigned long day;
  unsigned long month;
  unsigned long year;
} Time;

/**
 * @brief Initializes the Time struct.
 * @return timer
 */
Time* initTime();
/**
 * @brief Subscribes and enables the RTC interrupts.
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int rtc_subscribe_int();

/**
 * @brief Unsubscribes the RTC interrupts.
 * @return return 0 upon success and -1 otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief Deletes the time.
 * @param Time
 */
void deleteTime(Time* time);

/**
 * @brief Get the current time and date.
  * @param Time
 */
void getCurrentTime(Time* t);

/**
 * @brief Converts values from bcd to decimal.
 */
unsigned long bcdToDec(unsigned long bcd);

/** @} end of RTC */

#endif /* __RTC_H */
