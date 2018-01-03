#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"
#include "timer.h"

int counter = 0;
int hook_ID = 0;

int timer_get_conf(unsigned char timer, unsigned char *st);

int timer_set_frequency(unsigned char timer, unsigned long freq) {

	if (timer < 0 || timer > 2) { // timer must be 0,1 or 2

		return 1;

	}

	if (freq < 19 || freq > TIMER_FREQ) { //the lowest frequency: TIMER_FREQ/2¹⁷-1 (the number of bits)

		return 1;

	}

	unsigned char st;

	unsigned int status = timer_get_conf(timer, &st);

	if (status != 0) {

		return 1;
	}

	unsigned char fourLSB = st & (BIT(3) | BIT(2) | BIT(1) | BIT(0));

	unsigned long controlWord = TIMER_LSB_MSB | fourLSB;



	//Select timer and config Control Word


	switch (timer) {
	case 0:
		controlWord |= TIMER_SEL0;
		break;
	case 1:
		controlWord |= TIMER_SEL1;
		break;
	case 2:
		controlWord |= TIMER_SEL2;
		break;
	}

	if (sys_outb(TIMER_CTRL, controlWord) != OK) {

		return 1;

	}

	unsigned long div = TIMER_FREQ / freq;

	unsigned char lsb = (unsigned char) div;

	unsigned char msb = (unsigned char) (div >> 8);

	switch (timer) {
	case 0:
		if (sys_outb(TIMER_0, lsb) != OK) {

			return 1;

		}
		if (sys_outb(TIMER_0, msb) != OK) {

			return 1;

		}
		break;
	case 1:
		if (sys_outb(TIMER_1, lsb) != OK) {

			return 1;

		}
		if (sys_outb(TIMER_1, msb) != OK) {

			return 1;
		}
		break;
	case 2:
		if (sys_outb(TIMER_2, lsb) != OK) {

			return 1;

		}
		if (sys_outb(TIMER_2, msb) != OK) {

			return 1;

		}
		break;
	}

	return 0;
}

int timer_subscribe_int(void) {

	int hook_TEMP = hook_ID;

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_ID) != OK) {

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

void timer_int_handler() {

	counter++;

}

int timer_get_conf(unsigned char timer, unsigned char *st) {

	if (timer < 0 || timer > 2) {

		return 1;

	}

	// Read-Back Command 11000000
	// Count             00100000
	// Status            00010000

	// Select Counter 2  00001000
	// Select Counter 1  00000100
	// Select Counter 0  00000010

	// Reserved          00000000

	unsigned long timerConf = TIMER_RB_CMD | TIMER_RB_COUNT_
			| TIMER_RB_SEL(timer);

	unsigned long byte;

	int timerStatus;

	timerStatus = sys_outb(TIMER_CTRL, timerConf);

	if (timerStatus != 0) // confirm the timer_ctrl

		return 1;

	timerStatus = sys_inb(TIMER_0 + timer, &timerConf);

	*st = (unsigned char) timerConf;

	if (timerStatus != 0)

		return 1;

	return 0;
}

int timer_display_conf(unsigned char conf) {

	int n;

	n = conf & BIT(0);

	if (n == 1)

		printf("\n Counting Mode : BCD \n");

	else
		printf("\n Counting Mode : Binary \n");

	n = (conf & (BIT(3) | BIT(2) | BIT(1))) >> 1;

	if (n == 2 || n == 6)

		printf(" Programmed Mode : 2\n");

	else if (n == 3 || n == 7)

		printf(" Programmed Mode : 3\n");

	else
		printf(" Programmed Mode : %u \n", n);

	n = (conf & (BIT(5) | BIT(4))) >> 4;

	if (n == 1)

		printf(" Counter Initialization : LSB\n");

	else if (n == 2)

		printf(" Counter Initialization : MSB\n");

	else
		printf(" Counter Initialization : LSB followed by MSB\n");

	n = conf & BIT(6);

	if (n == 64)

		printf(" Null Count : 1\n");

	else
		printf(" Null Count : 0\n");

	n = conf & BIT(7);

	if (n == 128)

		printf(" Output pin : 1\n\n");

	else
		printf(" Output pin : 0\n\n");

	return 0;
}

int timer_test_time_base(unsigned long freq) {

	timer_set_frequency(0, freq);

	return 0;
}

int timer_test_int(unsigned long time) {

	int ipc_status;
	int irq_set = timer_subscribe_int();
	int r;
	message msg;

	while (time * 60 > counter) { /* You may want to use a different condition */
		/* Get a request message. */

		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {

			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

				timer_int_handler();

				if(counter % 60 == 0){

					printf("One second has gone by\n");

				}
			}

			break;

		default:

			break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	return 0;
}

int timer_test_config(unsigned char timer) {

	unsigned char st;

	timer_get_conf(timer, &st);
	timer_display_conf(st);

	return 0;
}
