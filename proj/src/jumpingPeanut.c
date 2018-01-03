#include <minix/drivers.h>
#include "jumpingPeanut.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "game.h"
#include "i8042.h"
#include "i8254.h"
#include "stateMachine.h"

JumpingPeanut*
startJumpingPeanut() {  // returns a pointer to a JumpingPeanut "object".

  // allocate space for a new JumpingPeanut
  JumpingPeanut* peanut = (JumpingPeanut*)malloc(sizeof(JumpingPeanut));

  // subscribe to activate interruptions
  peanut->kbd_irq = kbd_subscribe();
  peanut->mouse_irq = mouse_subscribe();
  peanut->timer_irq = timer_subscribe_int();
  peanut->rtc_irq = rtc_subscribe_int();

  // load menus and heart bitmaps
  peanut->menu = loadBitmap("/home/lcom/proj/res/menu.bmp");
  peanut->gameOver = loadBitmap("/home/lcom/proj/res/gameover.bmp");
  peanut->heart = loadBitmap("/home/lcom/proj/res/heart.bmp");

  // load all the numbers bitmaps
  peanut->zero = loadBitmap("/home/lcom/proj/res/0.bmp");
  peanut->one = loadBitmap("/home/lcom/proj/res/1.bmp");
  peanut->two = loadBitmap("/home/lcom/proj/res/2.bmp");
  peanut->three = loadBitmap("/home/lcom/proj/res/3.bmp");
  peanut->four = loadBitmap("/home/lcom/proj/res/4.bmp");
  peanut->five = loadBitmap("/home/lcom/proj/res/5.bmp");
  peanut->six = loadBitmap("/home/lcom/proj/res/6.bmp");
  peanut->seven = loadBitmap("/home/lcom/proj/res/7.bmp");
  peanut->eigth = loadBitmap("/home/lcom/proj/res/8.bmp");
  peanut->nine = loadBitmap("/home/lcom/proj/res/9.bmp");
  peanut->points = loadBitmap("/home/lcom/proj/res/points.bmp");

  // load the player variables
  peanut->x_player = PLAYER_START_X;
  peanut->y_player = PLAYER_START_Y;
  peanut->new_y_player = PLAYER_START_Y;
  peanut->jump_player = 0;
  peanut->score_player = 0;
  peanut->life_player = 3;
  peanut->collision_player = 0;
  peanut->out_boundaries = 0;

  // initialize the object variables
  peanut->done = 0;
  peanut->draw = 1;
  peanut->key = 0;

  // return the pointer to the newly created JumpingPeanut
  return peanut;
}

int updateJumpingPeanut(JumpingPeanut* peanut, State* state, Time* timer) {
  message msg;
  int r, i;
  int ipc_status, j = 60 / FPS;
  static unsigned int counter = 0;

  if (j <= 0) return -1;

  if (clear_outbuf() == -1) return -1;

  mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING);  // enable of the data reporting

  mouse_cmd_maker(MOUSE_SET_REMOTEMODE);  // change the mouse to remote

  mouse_cmd_maker(MOUSE_SET_STREAM);

  enable_mouse_interrupts();

  while (state->currentState == GAME) {
    r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0) {
      printf("driver_receive failed with: %d", r);

      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */

      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */

          if (msg.NOTIFY_ARG & peanut->mouse_irq) {
            update_mouse();
          }
          if (msg.NOTIFY_ARG & peanut->kbd_irq) { /* subscribed interrupt */

              game_kbd(peanut, state);
          }

          if (msg.NOTIFY_ARG & peanut->timer_irq) { /* subscribed interrupt */

            counter++;

            if (counter % 315 == 0) {
              game_clouds(peanut);
            }

            if (peanut->jump_player == 0) {
              peanut->new_y_player += 2;
            }

            if (counter % j == 0) {

              for (i = 0; i < 100; ++i) {
                if (peanut->clouds[i] != NULL) {
                  if(peanut->life_player < 3){
                if(peanut->score_player % 25 == 0 && peanut->score_player != 0 && peanut->x_player == peanut->clouds[i]->clouds_x + peanut->clouds[i]->width){
                  peanut->life_player++;
                }
              }
            }
          }

              for (i = 0; i < 100; ++i) {
                if (peanut->clouds[i] != NULL) {
                  peanut->clouds[i]->clouds_x -=
                      peanut->clouds[i]->clouds_speed;
                }
              }

              for (i = 0; i < 100; ++i) {
                if (peanut->clouds[i] != NULL) {
                  if(peanut->clouds[i]->clouds_x <= -300){
                    deleteClouds(peanut->clouds[i]);
                    peanut->clouds[i] = NULL;
                  }
                }
              }

              if (get_mouse()->left_button_d == 1 &&
                  get_mouse()->delta_y > 200) {
                peanut->new_y_player -= 15;
                peanut->jump_player = 1;
              }

              game_vertical_move(peanut, timer);

              if (peanut->collision_player == 1) {
                if (peanut->clouds[peanut->collision_cloud] != NULL) {
                  if (peanut->x_player >=
                      peanut->clouds[peanut->collision_cloud]->clouds_x + 280) {
                    peanut->collision_player = 0;
                  }
                }
              }

              if (peanut->out_boundaries == 1) {
                if (peanut->y_player > 0 && peanut->y_player < V_RES) {
                  peanut->out_boundaries = 0;
                }
              }

              if (peanut->collision_player == 0) {
                game_collision(peanut, state);
              }

              game_score(peanut);

              if (peanut->y_player == peanut->new_y_player) {
                peanut->jump_player = 0;

              }
            }
          }

          break;

        default:

          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
             /* no standard messages expected: do nothing */
    }

    if (state->currentState != GAME) {
      break;
    }
  }

  if (clear_outbuf() == -1) return -1;

  return 0;
}

void drawJumpingPeanut(JumpingPeanut* peanut, Time* timer) {
  static int n = 0;
  int i;

  bitmapdrawerBackground(peanut->background, 0, 0);
  for (i = 0; i < 100; ++i) {
    if (peanut->clouds[i] != NULL) {
      drawClouds(peanut->clouds[i]);
    }
  }

  if(peanut->collision_player == 0){
  if (n % 4 == 0) {
    bitmapDrawer(peanut->image_1, peanut->x_player, peanut->y_player);
    n++;
  } else {
    bitmapDrawer(peanut->image_2, peanut->x_player, peanut->y_player);
    n++;
  }
} else if(peanut->collision_player == 1){
  if (n % 3 == 0) {
    n++;
  } else {
    bitmapDrawer(peanut->image_1, peanut->x_player, peanut->y_player);
    n++;
  }
}

  game_draw_score(peanut);

  game_draw_life(peanut);

  game_draw_time(peanut, timer);

  doubleBufferToVideoMem();
}

void stopJumpingPeanut(JumpingPeanut* peanut) {

  kbd_unsubscribe();
  mouse_unsubscribe();
  timer_unsubscribe_int();
  clear_outbuf();
  game_delete_bitmaps(peanut);

  free(peanut);
}
