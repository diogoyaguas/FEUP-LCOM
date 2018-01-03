#include "test3.h"
#include "kbd.h"
#include "i8042.h"

int hook_ID_KBD = 1;

unsigned int counter_cmd;

#ifdef LAB3
int sys_inb_cnt(port_t port, unsigned long *byte) {
	counter_cmd++;
	return sys_inb_cnt(port,byte);
}
#else
#define sys_inb_cnt(p,q) sys_inb(p,q)
#endif

int kbd_subscribe(void) {

	int hook_TEMP_KBD = hook_ID_KBD;

	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_ID_KBD)
			!= OK) {
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
		if (read_status_reg(&status) == -1)
			return -1;

		if ((status & BIT(0)) == 1) {
			if (sys_inb(KBD_OUT_BUF, byte) != OK)
				return -1;
			return 0;
		}

		tickdelay(micros_to_ticks(DELAY_US));
	}

	return 0;
}

int read_status_reg(unsigned long *byte) {

	if (sys_inb(STATUS_PORT, byte) != OK)
		return -1;

	counter_cmd++;

	return 0;
}

void handler_c() {

	unsigned long byte;

	if (read_outbuf(&byte) == -1) {

		return;

	} else {

		key = byte;

		write_code(byte);
	}
}

int handler_c_poll(unsigned char * scancode) {

	unsigned long readoutbuf, status;

	unsigned int tries;

	for (tries = 0; tries < 5; tries++) {

		if (sys_inb(STATUS_PORT, &status) != OK)
			continue; //try again

		if (status & BIT(0)) {

			if (status & (PARITY | TIMEOUT)) { // parity and timeout detection

				return -1;

				if (sys_inb(KBD_OUT_BUF, &readoutbuf) != OK) {

					return -1;

				} else {

					*scancode = (unsigned char) readoutbuf;
					return 0;
				}
			}

		}
	}
	return 1;
}

void write_code(unsigned long byte) {

	static int tscan = 0;

	if (tscan == 1) { //the first byte of two byte scancodes is usually 0xE0

		if (byte & BIT(7)) {

			printf("Breakcode: 0xE0 0x%X\n", byte);

		} else

			printf("\nMakecode: 0xE0 0x%X\n", byte);

		tscan = 0;
	}

	else {

		if (byte == SPC_KEY) {

			tscan = 1;
			return;
		}

		if (byte & BIT(7)) {

			printf("Breakcode:0x%X\n", byte);

		} else

			printf("\nMakecode:0x%X\n", byte);

	}
}

int read_outb_cmd(unsigned long cmd, unsigned long address) {

	unsigned long byte;

	while (1) {

		if (read_status_reg(&byte) == -1) {

			return -1;
		}

		if ((byte & BIT(1)) == 0) {

			if (sys_outb(address, cmd) != OK) {

				return -1;
			} else
				return 0;
		}

		tickdelay(micros_to_ticks(DELAY_US));

	}

	return 0;

}

int kbd_poll() {

	unsigned long byte;
	unsigned long scancode;
	int handler_errorlevel;

	while (scancode != ESC_KEY) {

		handler_errorlevel = read_outbuf(&scancode);

		//Fail
		if (handler_errorlevel == -1)
			return -1;

		//Successfully read, print
		if (handler_errorlevel == 0)
			write_code(scancode);

	}

	printf("\nESC pressed\n");

	printf("Number of sys_inb kernel calls: %d\n", counter_cmd);

	//read_outb_cmd(KBD_OUT_BUF, KBD_CMD_BUF);
	//read_outb_cmd(byte | OBF, KBD_OUT_BUF);

	return 0;
}
