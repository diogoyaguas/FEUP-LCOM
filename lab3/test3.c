#include "test3.h"
#include "kbd.h"
#include "i8042.h"
#include "timer.h"

int kbd_counter = 0;

unsigned long key;

unsigned long handler_a();

int kbd_test_scan(unsigned short assembly) {

	//read and display the scancodes from the KBC (make codes and break codes)

	message msg;
	int ipc_status;

	int irq_set = kbd_subscribe();

	if (irq_set == -1) {

		return -1;
	}

	// Upon an interrupt, read the scancode from the OUT_BUF

	while (key != ESC_KEY) { //Should terminate when it reads the breakcode of the ESC key: 0x81

		/* Get a request message. */

		int r = driver_receive(ANY, &msg, &ipc_status);

		if (r != 0) {

			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					if (assembly == 0) {

						handler_c();

					} else {

						key = handler_a();
						write_code(key);

					}

					break;
				}
			default:

				break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (kbd_unsubscribe() == -1)

		return -1;

	printf("\nESC pressed\n");

	if (assembly == 0) {

		printf("Number of sys_inb kernel calls: %d\n", counter_cmd);

	}

	return 0;
}

int kbd_test_poll() {

	kbd_poll();
	return 0;
}

int kbd_test_timed_scan(unsigned short n) {

//read and display the scancodes from the KBC (make codes and break codes)

	message msg;
	int ipc_status;
	int irq_set_kbd = kbd_subscribe();
	int irq_set_timer = timer_subscribe_int();
	unsigned int counter_kbd = 0;

	if (n <= 0)

		return -1;

// Upon an interrupt, read the scancode from the OUT_BUF

	while (key != ESC_KEY) { //Should terminate when it reads the breakcode of the ESC key: 0x81

		/* Get a request message. */

		int r = driver_receive(ANY, &msg, &ipc_status);

		if (r != 0) {

			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */

					counter_kbd++;

					if (counter_kbd == n * 60.0) {

						printf("\n%u seconds past by", n);

						return -1;
					}

				}

				if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
					counter_kbd = 0;
					handler_c();

				}
				break;

			default:

				break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (kbd_unsubscribe() == -1)

		return -1;

	printf("\nESC pressed\n");

	return 0;
}
