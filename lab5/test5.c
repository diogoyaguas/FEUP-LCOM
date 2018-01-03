#include "test5.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/driver.h>
#include "lmlib.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "kbd.h"
#include "video_test.h"
#include "sprite.h"
#include "i8042.h"
#include "i8254.h"
#include <stdint.h>
#include <machine/int86.h>

void *video_test_init(unsigned short mode, unsigned short delay) {
  char *video_mem;

  video_mem = vg_init(mode);

  vbe_mode_info_t mode_info;

  if (video_mem == NULL) {
    vg_exit();
    return NULL;
  }

  if (vbe_get_mode_info(mode, &mode_info) == 1) return NULL;

  timer_test_int(delay);

  if (vg_exit() == -1) return NULL;

  return (void *)video_mem;
}

int video_test_square(unsigned short x, unsigned short y, unsigned short size,
                      unsigned long color) {
  vg_init(0x105);

  if (draw_square(x, y, size, color) != 0) {
    vg_exit();
    return -1;
  }

  if (kbd_ESC_key() != 0) {
    return -1;
  }

  if (vg_exit() != 0) {
    return -1;
  }

  return 0;
}

int video_test_line(unsigned short xi, unsigned short yi, unsigned short xf,
                    unsigned short yf, unsigned long color) {
  vg_init(0x105);

  if (draw_line(xi, yi, xf, yf, color) != 0) {
    vg_exit();
    return -1;
  }

  if (kbd_ESC_key() != 0) {
    return -1;
  }

  if (vg_exit() != 0) {
    return -1;
  }

  return 0;
}

int video_test_xpm(char *xpm[], unsigned short xi, unsigned short yi) {
  vg_init(0x105);

  if (draw_xpm(xpm, xi, yi) != 0) {
    vg_exit();
    return -1;
  }

  if (kbd_ESC_key() != 0) {
    return -1;
  }

  if (vg_exit() != 0) {
    return -1;
  }

  return 0;
}

int video_test_move(char *xpm[], unsigned short xi, unsigned short yi,
                    unsigned short xf, unsigned short yf, short speed,
                    unsigned short frame_rate) {
  message msg;
  int r;
  int ipc_status, j = 60 / frame_rate;
  int irq_set_kbd = kbd_subscribe();
  int irq_set_timer = timer_subscribe_int();
  unsigned int counter = 0;

  Sprite *sp;

  if (j <= 0) return -1;

  if (xf - xi == 0) {
    sp = create_sprite(xpm, xi, yi, 0, speed);
  }

  else if (yf - yi == 0) {
    sp = create_sprite(xpm, xi, yi, speed, 0);
  }

  else
    return -1;

  vg_init(0x105);

  while (key != ESC_KEY) {  // Should terminate when it reads the breakcode of
                            // the ESC key: 0x81

    /* Get a request message. */

    r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0) {
      printf("driver_receive failed with: %d", r);

      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */

      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */

          if (msg.NOTIFY_ARG & irq_set_timer) { /* subscribed interrupt */
            counter++;

            if (counter % j == 0) {
              if (speed < 0) {  //  If the speed is negative it should be
                                //  understood as the number of
                //  frames required for a displacement of one pixel.

                if (sp->x < xf && yi == yf) {
                  erase_sprite(sp);
                  sp->x++;
                  draw_sprite(sp);
                }

                else if (xi == xf && sp->y < yf) {
                  erase_sprite(sp);
                  sp->y++;
                  draw_sprite(sp);
                }

                else if (sp->x > xf && yi == yf) {
                  erase_sprite(sp);
                  sp->x--;
                  draw_sprite(sp);
                }

                else if (xi == xf && sp->y > yf) {
                  erase_sprite(sp);
                  sp->y--;
                  draw_sprite(sp);
                }
              } else {  //  If speed is positive it should be understood as the
                        //  displacement
                        //  in pixels between consecutive frames.

                if (sp->x < xf && yi == yf) {
                  erase_sprite(sp);
                  sp->x += sp->xspeed;
                  draw_sprite(sp);
                }

                else if (xi == xf && sp->y < yf) {
                  erase_sprite(sp);
                  sp->y += sp->yspeed;
                  draw_sprite(sp);
                }

                else if (sp->x > xf && yi == yf) {
                  erase_sprite(sp);
                  sp->x += -1 * sp->xspeed;
                  draw_sprite(sp);
                }

                else if (xi == xf && sp->y > yf) {
                  erase_sprite(sp);
                  sp->y += -1 * sp->yspeed;
                  draw_sprite(sp);
                }
              }
            }
          }

          if (msg.NOTIFY_ARG & irq_set_kbd) { /* subscribed interrupt */
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

  if (timer_unsubscribe_int() == -1) return -1;

  if (kbd_unsubscribe() == -1) return -1;

  destroy_sprite(sp);

  if (vg_exit() != 0) {
    return -1;
  }

  return 0;
}

int test_controller() {
  vbe_get_info_block();

  return 0;
}
