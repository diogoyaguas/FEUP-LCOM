#include "test4.h"
#include "mouse.h"
#include "timer.h"
#include "i8042.h"

unsigned long packet_array[3];

int mouse_test_packet(unsigned short cnt) {

	//test that your code is able to read the packets from the PS/2 mouse using an interrupt handler.

	int nmb_packets = 0, r;

	unsigned long byte;

	message msg;

	int ipc_status;

	if (clear_outbuf() == -1) return -1;

	int irq_set = mouse_subscribe();

	if (irq_set == -1) {

		return -1;
	}

	if (mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING) == -1) return -1;


	while (nmb_packets != cnt) {
		/* Get a request message. */

		r = driver_receive(ANY, &msg, &ipc_status);

		if (r != 0) {

			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

						byte = mouse_handler();

						if(counter_bytes%3 == 0 && (BIT(3) & byte) == BIT(3)){

							packet_array[0] = byte;
							counter_bytes++;
						}

						else if(counter_bytes%3 == 1){

							packet_array[1] = byte;
							counter_bytes++;
						}

						else if(counter_bytes%3 == 2){

							packet_array[2] = byte;
							print_packet();
							counter_bytes = 0;
							nmb_packets++;
						}

				}
			default:

				break; /* no other notifications expected: do nothing */
			}

		} else {

			printf("No interruptions");/* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (mouse_cmd_maker(MOUSE_DISABLE_DATA_REPORTING) == -1) return -1;

	if (clear_outbuf() == -1) return -1;

	if (mouse_unsubscribe() == -1) return -1;

		printf("\n");

	return 0;
}

int mouse_test_async(unsigned short idle_time) {

	int nmb_packets = 0, r;

	unsigned long byte;

	message msg;

	unsigned long counter_mouse = 0, total = idle_time * 60;

	int ipc_status;

	int irq_set_mouse = mouse_subscribe();

	int irq_set_timer = timer_subscribe_int();

	if (irq_set_timer == -1) {

		return -1;
	}

	if (irq_set_mouse == -1) {

		return -1;
	}

	if (mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING) == -1) return -1;

	while (counter_mouse < total) {

		/* Get a request message. */

		r = driver_receive(ANY, &msg, &ipc_status);

		if (r != 0) {

			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set_mouse) { /* subscribed interrupt */

					counter_mouse = 0;

						byte = mouse_handler();

						if(counter_bytes%3 == 0 && (BIT(3) & byte) == BIT(3)){

							packet_array[0] = byte;
							counter_bytes++;
						}

						else if(counter_bytes%3 == 1){

							packet_array[1] = byte;
							counter_bytes++;
						}

						else if(counter_bytes%3 == 2){

							packet_array[2] = byte;
							print_packet();
							counter_bytes = 0;
							nmb_packets++;
						}

				}

				if(msg.NOTIFY_ARG & irq_set_timer){

					counter_mouse++;

					if (counter_mouse == idle_time * 60)

						printf("\n\n%u seconds past by", idle_time);

				}

			default:

				break; /* no other notifications expected: do nothing */
			}

		} else {

			printf("No interruptions");/* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	if (mouse_cmd_maker(MOUSE_DISABLE_DATA_REPORTING) == -1)

		return -1;

	if (clear_outbuf() == -1) return -1;

	if (timer_unsubscribe_int() == -1) return -1;

	if (mouse_unsubscribe() == -1) return -1;

		printf("\n");

	return 0;
}

int mouse_test_remote(unsigned long period, unsigned short cnt) {

	int nmb_packets = 0;

	disable_mouse_interrupts(); //disable the interrupts of the mouse (read the command byte, put the BIT(1) = 0 and rewrite the command byte)

	mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING); //enable of the data reporting

	mouse_cmd_maker(MOUSE_DISABLE_DATA_REPORTING); //disable of the data reporting

	mouse_cmd_maker(MOUSE_SET_REMOTEMODE); //change the mouse to remote

	while(nmb_packets != cnt){

	mouse_cmd_maker(MOUSE_READ_DATA); //send the request data packet

		packet_array[0] = mouse_handler();

		tickdelay(micros_to_ticks(DELAY_US));

		packet_array[1] = mouse_handler();

		tickdelay(micros_to_ticks(DELAY_US));

		packet_array[2] = mouse_handler();

		tickdelay(micros_to_ticks(DELAY_US));

		print_packet(); //write the data packet

		nmb_packets++;

		tickdelay(micros_to_ticks(period*1000));
	}

	mouse_cmd_maker(MOUSE_SET_STREAM); //change to stream

	enable_mouse_interrupts(); //reactivate the interrupts of the mouse

	if (clear_outbuf() == -1) return -1;

	return 0;
}

int mouse_test_gesture(short length) {

	int nmb_packets = 0, r;

	long x,y;

	unsigned long byte;

	short total = 0;

	message msg;

	int ipc_status;

	static state_t st = INIT;

	if (clear_outbuf() == -1) return -1;

	int irq_set = mouse_subscribe();

	if (irq_set == -1) {

		return -1;
	}

	if (mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING) == -1) return -1;


	while (st != COMP) {

		/* Get a request message. */

		r = driver_receive(ANY, &msg, &ipc_status);

		if (r != 0) {

			printf("driver_receive failed with: %d", r);

			continue;
		}

		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {

			case HARDWARE: /* hardware interrupt notification */

				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */

					byte = mouse_handler();

					if(counter_bytes%3 == 0 && (BIT(3) & byte) == BIT(3)){

						packet_array[0] = byte;
						counter_bytes++;
					}

					else if(counter_bytes%3 == 1){

						packet_array[1] = byte;
						counter_bytes++;
					}

					else if(counter_bytes%3 == 2){

						packet_array[2] = byte;
						print_packet();
						counter_bytes = 0;
						nmb_packets++;
					}

					if((packet_array[0] & X_SIGN) == X_SIGN){

						x = ~packet_array[1] +1;
					}
					else x = packet_array[1];

					if((packet_array[0] & Y_SIGN) == Y_SIGN){

						y = ~packet_array[2] +1;
					}
					else y = packet_array[2];

					if(st == INIT && (packet_array[0] & BIT(1)) == 1){

						check_hor_line(RDOWN, &x, &y, &length, &total, &st);

					}
					else if ((packet_array[0] & BIT(1)) == 0){

					check_hor_line(RUP, &x , &y , &length, &total, &st);

				}

				else if(st == DRAW){

					check_hor_line(MOVE, &x, &y, &length, &total, &st);
				}

				}
			default:

				break; /* no other notifications expected: do nothing */
			}

		} else {

			printf("No interruptions");/* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}
	if (mouse_cmd_maker(MOUSE_DISABLE_DATA_REPORTING) == -1) return -1;

	if (clear_outbuf() == -1) return -1;

	if (mouse_unsubscribe() == -1) return -1;

		printf("\n");

	return 0;
}
