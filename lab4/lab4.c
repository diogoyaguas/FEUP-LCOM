#include "test4.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);
static unsigned long parse_long(char *str, int base);

int main(int argc, char **argv) {
	sef_startup(); // Initialization of Privileged User

	if (argc == 1) { /* Prints usage of the program if no arguments are scaned */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv) {
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"packet <decimal no. - number of packets to receive>\"\n"
					"\t service run %s -args \"async <decimal no. - seconds without packets until break>\"\n"
					"\t service run %s -args \"remote <decimal no. - period in ms> <decimal no. - number of packets to receive>\"\n"
					"\t service run %s -args \"gesture <decimal no. - minimum length of movement in the x direction>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {

	unsigned long packet, async, period, length;

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments mouse_test_packet()\n");
			return 1;
		}
		packet = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (packet == ULONG_MAX)
			return 1;
		printf("mouse::mouse_test_packet(%lu)\n", packet);
		return mouse_test_packet(packet);
	}

	else if (strncmp(argv[1], "remote", strlen("remote")) == 0) {
		if (argc != 4) {
			printf("mouse: wrong no of arguments for mouse_test_remote()\n");
			return 1;
		}
		period = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		packet = parse_ulong(argv[3], 10); /* Parses string to unsigned long */
		if (period == ULONG_MAX)
			return 1;
		if (packet == ULONG_MAX)
			return 1;
		printf("mouse::mouse_test_remote(%lu, %lu)\n", period, packet);
		return mouse_test_remote(period, packet);
	}

	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no of arguments for mouse_test_gesture()\n");
			return 1;
		}
		length = parse_long(argv[2], 10); /* Parses string to unsigned long */
		if (length == LONG_MAX || length == LONG_MIN)
			return 1;
		printf("mouse::mouse_test_gesture(%lu)\n", length);
		return mouse_test_gesture(length);
	}

	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no of arguments for mouse_test_async()\n");
			return 1;
		}
		async = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (async == ULONG_MAX)
			return 1;
		printf("mouse::mouse_test_async(%lu)\n", async);
		return mouse_test_async(async);
	}

	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("kbd: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}

static unsigned long parse_long(char *str, int base) {
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtol(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == LONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if ((errno == ERANGE && val == LONG_MIN) || (errno != 0 && val == 0)) {
		perror("strtol");
		return LONG_MAX;
	}

	if (endptr == str) {
		printf("kbd: parse_long: no digits were found in %s\n", str);
		return LONG_MAX;
	}

	/* Successful conversion */
	return val;
}
