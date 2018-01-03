#include "test5.h"
#include "pixmap.h"
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);
static unsigned long parse_long(char *str, int base);

int main(int argc, char **argv) {
  sef_startup();   // Initialization of Privileged User
  if (argc == 1) { /* Prints usage of the program if no arguments are scaned */
    print_usage(argv);
    return 0;
  } else
    return proc_args(argc, argv);
}

static void print_usage(char **argv) {
  printf(
      "Usage: one of the following:\n"
      "\t service run %s -args \"init <mode,delay>\"\n"
      "\t service run %s -args \"square <x,y,size,color>\"\n"
      "\t service run %s -args \"line <xi,yi,xf,yf,xpm,color>\"\n"
      "\t service run %s -args \"xpm <xpm,xi,yi>\"\n"
      "\t service run %s -args \"move <xpm,xi,yi,xf,yf,speed,frame_rate>\"\n"
      "\t service run %s -args \"controller <>\"\n",
      argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
  unsigned short x, y, size, xi, yi, xf, yf, frame_rate, mode, delay;
  short speed;
  unsigned long color;

  if (strncmp(argv[1], "init", strlen("init")) == 0) {
    if (argc != 4) {
      printf("graphicmode: wrong no. of arguments *video_test_init()\n");
      return 1;
    }

    mode = parse_ulong(argv[2], 16);
    if (mode == ULONG_MAX) return 1;

    delay = parse_ulong(argv[3], 10);
    if (delay == ULONG_MAX) return 1;

    printf("graphicmode::video_test_init(0x%lu, %lu)\n", mode, delay);
    return (int)video_test_init(mode, delay);
  } else if (strncmp(argv[1], "square", strlen("square")) == 0) {
    if (argc != 6) {
      printf("graphicmode: wrong no of arguments for video_test_square()\n");
      return 1;
    }
    x = parse_ulong(argv[2], 10);
    if (x == ULONG_MAX) return 1;
    y = parse_ulong(argv[3], 10);
    if (y == ULONG_MAX) return 1;
    size = parse_ulong(argv[4], 10);
    if (size == ULONG_MAX) return 1;
    color = parse_ulong(argv[5], 10);
    if (color == ULONG_MAX) return 1;
    printf("graphicmode::video_test_square(%lu, %lu, %lu, %lu)\n", x, y, size,
           color);
    return video_test_square(x, y, size, color);
  } else if (strncmp(argv[1], "line", strlen("line")) == 0) {
    if (argc != 7) {
      printf("graphicmode: wrong no of arguments for video_test_line()\n");
      return 1;
    }
    xi = parse_ulong(argv[2], 10);
    if (xi == ULONG_MAX) return 1;
    yi = parse_ulong(argv[3], 10);
    if (yi == ULONG_MAX) return 1;
    xf = parse_ulong(argv[4], 10);
    if (xf == ULONG_MAX) return 1;
    yf = parse_ulong(argv[5], 10);
    if (yf == ULONG_MAX) return 1;
    color = parse_ulong(argv[6], 10);
    if (color == ULONG_MAX) return 1;
    printf("graphicmode::video_test_line(%lu, %lu, %lu, %lu, %lu)\n", xi, yi,
           xf, yf, color);
    return video_test_line(xi, yi, xf, yf, color);
  } else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
    if (argc != 5) {
      printf("graphicmode: wrong no of arguments for video_test_xpm()\n");
      return 1;
    }
    xi = parse_ulong(argv[3], 10);
    if (xi == ULONG_MAX) return 1;
    yi = parse_ulong(argv[4], 10);
    if (yi == ULONG_MAX) return 1;
    if (strncmp(argv[2], "pic1", strlen("pic1")) == 0) {
      printf("test5: video_test_xpm(pic1, %lu, %lu)\n", xi, yi);
      return video_test_xpm(pic1, xi, yi);
    }
    if (strncmp(argv[2], "pic2", strlen("pic2")) == 0) {
      printf("test5: video_test_xpm(pic2, %lu, %lu)\n", xi, yi);
      return video_test_xpm(pic2, xi, yi);
    }
    if (strncmp(argv[2], "cross", strlen("cross")) == 0) {
      printf("test5: video_test_xpm(cross, %lu, %lu)\n", xi, yi);
      return video_test_xpm(cross, xi, yi);
    }
    if (strncmp(argv[2], "pic3", strlen("pic3")) == 0) {
      printf("test5: video_test_xpm(pic3, %lu, %lu)\n", xi, yi);
      return video_test_xpm(pic3, xi, yi);
    }
    if (strncmp(argv[2], "penguin", strlen("penguin")) == 0) {
      printf("test5: video_test_xpm(penguin, %lu, %lu)\n", xi, yi);
      return video_test_xpm(penguin, xi, yi);
    } else {
      printf("test5: video_test_xpm:: unrecognized xpm\n");
      return 1;
    }
  } else if (strncmp(argv[1], "move", strlen("move")) == 0) {
    if (argc != 9) {
      printf("graphicmode: wrong no of arguments for test_move()\n");
      return 1;
    }
    xi = parse_ulong(argv[3], 10);
    if (xi == ULONG_MAX) return 1;
    yi = parse_ulong(argv[4], 10);
    if (yi == ULONG_MAX) return 1;
    xf = parse_ulong(argv[5], 10);
    if (xf == ULONG_MAX) return 1;
    yf = parse_ulong(argv[6], 10);
    if (yf == ULONG_MAX) return 1;
    speed = parse_long(argv[7], 10);
    if (speed == LONG_MAX || speed == LONG_MIN) return 1;
    frame_rate = parse_ulong(argv[8], 10);
    if (frame_rate == ULONG_MAX) return 1;
    if (strncmp(argv[2], "pic1", strlen("pic1")) == 0) {
      printf("test5: video_test_move(pic1, %lu, %lu, %lu, %lu, %lu, %lu)\n", xi,
             yi, xf, yf, speed, frame_rate);
      return video_test_move(pic1, xi, yi, xf, yf, speed, frame_rate);
    }
    if (strncmp(argv[2], "pic2", strlen("pic2")) == 0) {
      printf("test5: video_test_move(pic2, %lu, %lu, %lu, %lu, %lu, %lu)\n", xi,
             yi, xf, yf, speed, frame_rate);
      return video_test_move(pic2, xi, yi, xf, yf, speed, frame_rate);
    }
    if (strncmp(argv[2], "cross", strlen("cross")) == 0) {
      printf("test5: video_test_move(cross, %lu, %lu, %lu, %lu, %lu, %lu)\n",
             xi, yi, xf, yf, speed, frame_rate);
      return video_test_move(cross, xi, yi, xf, yf, speed, frame_rate);
    }
    if (strncmp(argv[2], "pic3", strlen("pic3")) == 0) {
      printf("test5: video_test_move(pic3, %lu, %lu, %lu, %lu, %lu, %lu)\n", xi,
             yi, xf, yf, speed, frame_rate);
      return video_test_move(pic3, xi, yi, xf, yf, speed, frame_rate);
    }
    if (strncmp(argv[2], "penguin", strlen("penguin")) == 0) {
      printf("test5: video_test_move(penguim, %lu, %lu, %lu, %lu, %lu, %lu)\n",
             xi, yi, xf, yf, speed, frame_rate);
      return video_test_move(penguin, xi, yi, xf, yf, speed, frame_rate);
    } else {
      printf("test5: video_test_move:: unrecognized xpm\n");
      return 1;
    }
  } else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
    if (argc != 2) {
      printf(
          "test5: Wrong number of arguments for test of test_controller()\n");
      return 1;
    }
    printf("test5: test_controller()\n");
    return test_controller();
  } else {
    printf("test5: %s - no valid function!\n", argv[1]);
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
    return LONG_MIN;
  }

  if (endptr == str) {
    printf("kbd: parse_long: no digits were found in %s\n", str);
    return LONG_MAX;
  }

  /* Successful conversion */
  return val;
}
