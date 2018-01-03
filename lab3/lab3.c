#include "test3.h"
#include "kbd.h"
#include "i8042.h"
#include "timer.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv) {
	sef_startup(); // Initialization of Privileged User

	sys_enable_iop(SELF);

	if (argc == 1) { /* Prints usage of the program if no arguments are pscaned */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);
}

static void print_usage(char **argv) {
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"\"\n"
			"\t service run %s -args \"\n"
			"\t service run %s -args \"\n", argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {

	unsigned long scan, time;

	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no. of arguments kbd_test_scan()\n");
			return 1;
		}
		scan = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (scan == ULONG_MAX)
			return 1;
		printf("kbd::kbd_test_scan(%lu)\n", scan);
		return kbd_test_scan(scan);
	}

	else if (strncmp(argv[1], "poll", strlen("poll")) == 0) {
		if (argc != 2) {
			printf("kbd: wrong no. of arguments for kbd_test_poll()\n");
			return 1;
		}
		printf("kbd::kbd_test_poll()\n");
		return kbd_test_poll();
	}

	else if (strncmp(argv[1], "timed", strlen("timed")) == 0) {
		if (argc != 3) {
			printf("kbd: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		time = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (time == ULONG_MAX)
			return 1;
		printf("kbd::kbd_test_timed_scan()\n", time);
		return kbd_test_timed_scan(time);
	} else {
		printf("kbd: %s - no valid function!\n", argv[1]);
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
