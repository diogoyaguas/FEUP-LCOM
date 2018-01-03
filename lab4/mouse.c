#include "test4.h"
#include "mouse.h"
#include "i8042.h"

int hook_ID_MOUSE = 1;

unsigned int counter_bytes = 0;

state_t st = INIT;

int mouse_subscribe(void) {

	int hook_TEMP_MOUSE = hook_ID_MOUSE;

	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_ID_MOUSE)!= OK) {
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

int mouse_cmd_maker(unsigned long cmd){

	unsigned long status, byte;

	do {
	if (sys_inb(STAT_REG, &status) != OK) return -1; // verify the status of the buffer

	if ((status & IBF) == 0) {

		if (sys_outb(CMD_BUF, WRT_BYTE_MOUSE) != OK) return -1; // allow to write in the mouse

		if(sys_outb(OUT_BUF, cmd) != OK) return -1; // enable to transmit data

		tickdelay(micros_to_ticks(DELAY_US));

		if(sys_inb(OUT_BUF, &byte) != OK) return -1;

	}

	} while(byte != MOUSE_ACK);

	return 0;

}

int disable_mouse_interrupts() {

	unsigned long status, byte;

		if (sys_inb(STAT_REG, &status) != OK) return -1; // verify the status of the buffer

		if ((status & IBF) == 0) {

			if(sys_outb(CMD_BUF, READ_CMD_BYTE) != OK) return -1; // ask permission to read the command byte

			if(sys_inb(OUT_BUF, &byte) != OK) return -1; // read the command byte

			byte = byte & BIT1_0; // sets BIT(1) to disable mouse interrupts

		}

		if(sys_outb(CMD_BUF, WRITE_CMD_BYTE) != OK) return -1; // ask permission to write

		if(sys_outb(IN_BUF, byte) != OK) return -1; // write the new command byte

		return 0;
}

int enable_mouse_interrupts() {

	unsigned long byte, status;


			if (sys_inb(STAT_REG, &status) != OK) return -1; // verify the status of the buffer

			if ((status & IBF) == 0) {

				if(sys_outb(CMD_BUF, READ_CMD_BYTE) != OK) return -1; // ask permission to read the command byte

				if(sys_inb(OUT_BUF, &byte) != OK) return -1; // read the command byte

				byte = byte | BIT(1); // sets BIT(1) to enable mouse interrupts

			}

			if(sys_outb(CMD_BUF, WRITE_CMD_BYTE) != OK) return -1; // ask permission to write

			if(sys_outb(IN_BUF, byte) != OK) return -1; // write the new command byte

	return 0;
}

unsigned long mouse_handler() {

	unsigned long byte;

	if (sys_inb(OUT_BUF, &byte) != OK) return -1;

	return byte;

}

void print_packet(){

	printf("\nB1=0x%02X ", packet_array[0]);
	printf("B2=0x%02X ", packet_array[1]);
	printf("B3=0x%02X", packet_array[2]);

	if((MOUSE_LB	 & packet_array[0]) == MOUSE_LB) // 0 ou 1 (0 BIT(0))
	printf(" L.B = %d", 1);
	else
	printf(" L.B = %d", 0);

	if((MOUSE_MB	 & packet_array[0]) == MOUSE_MB) // 0 ou 4 (0 BIT(2))
	printf(" M.B = %d", 1);
	else
	printf(" M.B = %d", 0);

	if((MOUSE_RB	 & packet_array[0]) == MOUSE_RB) // 0 ou 2 (0 BIT(1))
	printf(" R.B = %d", 1);
	else
	printf(" R.B = %d", 0);

	if((X_OVFL & packet_array[0]) == X_OVFL)
	printf(" Xof = %d", 1);
	else
	printf(" Xof = %d", 0);

	if((Y_OVFL & packet_array[0]) == Y_OVFL)
	printf(" Yof = %d", 1);
	else
	printf(" Yof = %d", 0);

	printf(" X=");
	if((X_SIGN & packet_array[0]) == X_SIGN){
	printf("%ld", (packet_array[1]) - 256);
	} else {
	printf("%lu", packet_array[1]);
	}

	printf(" Y=");
	if((Y_SIGN & packet_array[0]) == Y_SIGN){
	printf("%ld", (packet_array[2]) - 256);
	}
	else {
		printf("%lu", packet_array[2]);
	}

}

int clear_outbuf(){

	unsigned long status;

	if (sys_inb(STAT_REG, &status) != OK) return -1;

	while ((status & BIT(0)) != 0) {

		if (sys_inb(OUT_BUF	, &status) != OK) return -1;

		tickdelay(micros_to_ticks(DELAY_US));

		if (sys_inb(STAT_REG, &status) != OK) return -1;

	}

	return 0;

}

void check_hor_line(ev_type_t evt, long *x, long *y, short *length, short *total, state_t *st) {

	switch (*st) {
		case INIT:
			if( evt == RDOWN )
			*st = DRAW;
				break;
		case DRAW:
			if( evt == MOVE ) {
				if((*length > 0 && *x > 0 && *y > 0)){

					*total+= *x;

					if (*total >= *length){
						*st = COMP;
					}
				}
				else if((*length < 0 && *x < 0 && *y < 0)){

					*total+= *x;
					if (*total <= *length){
						*st = COMP;
					}
				}
				else{
				*total = 0;
				*st = INIT;
			}
		} else if( evt == RUP ){
				*st = INIT;
				*total = 0;
			}
			break;
		default:
				break;
			}
}
