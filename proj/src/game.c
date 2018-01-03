#include <minix/drivers.h>
#include "game.h"
#include "stateMachine.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"
#include "vbe.h"
#include "video_gr.h"
#include "i8042.h"
#include "i8254.h"
#include "cloud.h"

unsigned int n = 0;

void game_kbd(JumpingPeanut* peanut, State* state) {
  handler_c();
  peanut->key = key;

  if (peanut->key != 0) {
    if (peanut->key == ESC_KEY) {
      changeState(GAME_ESC, state);
    }

    if(peanut->key == P_KEY && state->currentState == MENU) {
      changeState(GAME_PLAY, state);
    }

    if ((peanut->key == SPC_KEY_1 || peanut->key == SPC_KEY_2) &&
        state->currentState == GAME) {
      peanut->new_y_player -= 15;
      peanut->jump_player = 1;
    }

    clear_outbuf();
  }
}

void game_menu(JumpingPeanut* peanut, State* state) {
  message msg;
  int r;
  int ipc_status, j = 60 / FPS;
  unsigned int counter = 0;

  if (j <= 0) return;

  if (clear_outbuf() == -1) return;

  mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING);  // enable of the data reporting

  mouse_cmd_maker(MOUSE_SET_REMOTEMODE);  // change the mouse to remote

  mouse_cmd_maker(MOUSE_SET_STREAM);

  enable_mouse_interrupts();

  while (state->currentState == MENU) {
    r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0) {
      printf("driver_receive failed with: %d", r);

      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */

      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */

          if (msg.NOTIFY_ARG & peanut->kbd_irq) { /* subscribed interrupt */
            game_kbd(peanut, state);
          }

          if (msg.NOTIFY_ARG & peanut->mouse_irq) { /* subscribed interrupt */
            update_mouse();
          }

          if (msg.NOTIFY_ARG & peanut->timer_irq) { /* subscribed interrupt */
            counter++;

            if (counter % j == 0) {
              bitmapdrawerBackground(peanut->menu, 0, 0);
              get_mouse();
              draw_mouse();
              game_menu_buttons(state);
              doubleBufferToVideoMem();
            }
          }

          break;

        default:

          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
             /* no standard messages expected: do nothing */
    }

    if (state->currentState != MENU) break;
  }

  if (clear_outbuf() == -1) return;

  return;
}

void game_menu_buttons(State* state) {
  // play
  if (get_mouse()->x >= 430 && get_mouse()->x <= 595 && get_mouse()->y >= 335 &&
      get_mouse()->y <= 375 && get_mouse()->left_button_r == 1) {
    changeState(GAME_PLAY, state);
  }
  // exit
  if (get_mouse()->x >= 430 && get_mouse()->x <= 595 && get_mouse()->y >= 445 &&
      get_mouse()->y <= 485 && get_mouse()->left_button_r == 1) {
    changeState(GAME_ESC, state);
  }
}

void game_gameOverMenu(JumpingPeanut* peanut, State* state) {
  message msg;
  int r;
  int ipc_status, j = 60 / FPS;
  unsigned int counter = 0;

  if (j <= 0) return;

  if (clear_outbuf() == -1) return;

  mouse_cmd_maker(MOUSE_ENABLE_DATA_REPORTING);  // enable of the data reporting

  mouse_cmd_maker(MOUSE_SET_REMOTEMODE);  // change the mouse to remote

  mouse_cmd_maker(MOUSE_SET_STREAM);

  enable_mouse_interrupts();

  while (state->currentState == GAMEOVER) {
    r = driver_receive(ANY, &msg, &ipc_status);

    if (r != 0) {
      printf("driver_receive failed with: %d", r);

      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */

      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */

          if(msg.NOTIFY_ARG & peanut->kbd_irq) {
            game_kbd(peanut, state);
          }

          if (msg.NOTIFY_ARG & peanut->mouse_irq) {
            update_mouse();
          }

          if (msg.NOTIFY_ARG & peanut->timer_irq) {
            counter++;

            if (counter % j == 0) {
              bitmapdrawerBackground(peanut->gameOver, 0, 0);
              get_mouse();
              draw_mouse();
              game_gameOver_buttons(state, peanut);
              doubleBufferToVideoMem();
            }
          }

          break;

        default:

          break; /* no other notifications expected: do nothing */
      }
    } else { /* received a standard message, not a notification */
             /* no standard messages expected: do nothing */
    }
    if (state->currentState != GAMEOVER) break;
  }

  if (clear_outbuf() == -1) return;

  return;
}

void game_gameOver_buttons(State* state, JumpingPeanut* peanut) {
  // try again
  if (get_mouse()->x >= 315 && get_mouse()->x <= 725 && get_mouse()->y >= 285 &&
      get_mouse()->y <= 340 && get_mouse()->left_button_r == 1) {
    changeState(GAME_PLAY, state);
    game_try_again(peanut);
  }
  // exit
  if (get_mouse()->x >= 420 && get_mouse()->x <= 600 && get_mouse()->y >= 440 &&
      get_mouse()->y <= 490 && get_mouse()->left_button_r == 1) {
    changeState(GAME_ESC, state);
  }
}

void game_vertical_move(JumpingPeanut* peanut, Time* timer) {
  if (peanut->y_player < peanut->new_y_player && peanut->jump_player == 0) {
    if ((peanut->y_player + GRAVITY) <= peanut->new_y_player) {
      peanut->y_player += GRAVITY;
    } else {
      peanut->y_player += peanut->new_y_player - peanut->y_player;
    }

    drawJumpingPeanut(peanut, timer);

  }

  else if (peanut->y_player > peanut->new_y_player) {
    if ((peanut->y_player - PLAYER_SPEED) >= peanut->new_y_player) {
      peanut->y_player -= PLAYER_SPEED;
    } else {
      peanut->y_player -= (peanut->y_player - peanut->new_y_player);
    }
    drawJumpingPeanut(peanut, timer);
  }
}

void game_clouds(JumpingPeanut* peanut) {
  int i;

  for (i = 0; i < 100; ++i) {
    if (peanut->clouds[i] == NULL) {
      Clouds* clouds = (Clouds*)startClouds();
      clouds->type = rand() % 5;
      if (i >= 1) {
        if (peanut->clouds[i - 1]->type == 0 && clouds->type == 4) {
          clouds->type = rand() % 4;
        }
      }
      peanut->clouds[i] = clouds;
      break;
    }
  }
}

void game_collision(JumpingPeanut* peanut, State* state) {
  int i;

  for (i = 0; i < 100; ++i) {
    if (peanut->clouds[i] != NULL) {
      switch (peanut->clouds[i]->type) {
        case 0:

          if (peanut->y_player >= peanut->clouds[i]->cloud2_y &&
              (peanut->x_player >= peanut->clouds[i]->clouds_x &&
               peanut->x_player <=
                   peanut->clouds[i]->clouds_x + peanut->clouds[i]->width)) {
            if (peanut->collision_player == 0) {
              if (peanut->life_player == 1) {
                changeState(GAME_COLLISION, state);
              } else {
                peanut->collision_player = 1;
                peanut->collision_cloud = i;
                peanut->life_player--;
              }
            }
          }

          break;

        case 1:

          if ((peanut->y_player <= (peanut->clouds[i]->cloud2_y - 30) ||
               peanut->y_player >= peanut->clouds[i]->cloud3_y) &&
              (peanut->x_player >= peanut->clouds[i]->clouds_x &&
               peanut->x_player <=
                   peanut->clouds[i]->clouds_x + peanut->clouds[i]->width)) {
            if (peanut->collision_player == 0) {
              if (peanut->life_player == 1)
                changeState(GAME_COLLISION, state);
              else {
                peanut->collision_player = 1;
                peanut->collision_cloud = i;
                peanut->life_player--;
              }
            }
          }

          break;

        case 2:

          if ((peanut->y_player <= (peanut->clouds[i]->cloud3_y - 50) ||
               peanut->y_player >= peanut->clouds[i]->cloud4_y - 10) &&
              (peanut->x_player >= peanut->clouds[i]->clouds_x &&
               peanut->x_player <=
                   peanut->clouds[i]->clouds_x + peanut->clouds[i]->width)) {
            if (peanut->collision_player == 0) {
              if (peanut->life_player == 1)
                changeState(GAME_COLLISION, state);
              else {
                peanut->collision_player = 1;
                peanut->collision_cloud = i;
                peanut->life_player--;
              }
            }
          }

          break;

        case 3:

          if ((peanut->y_player <= (peanut->clouds[i]->cloud4_y - 30) ||
               peanut->y_player >= peanut->clouds[i]->cloud5_y) &&
              (peanut->x_player >= peanut->clouds[i]->clouds_x &&
               peanut->x_player <=
                   peanut->clouds[i]->clouds_x + peanut->clouds[i]->width)) {
            {
              if (peanut->collision_player == 0) {
                if (peanut->life_player == 1)
                  changeState(GAME_COLLISION, state);
                else {
                  peanut->collision_player = 1;
                  peanut->collision_cloud = i;
                  peanut->life_player--;
                }
              }
            }

            break;

            case 4:

              if (peanut->y_player <= (peanut->clouds[i]->cloud5_y - 30) &&
                  (peanut->x_player >= peanut->clouds[i]->clouds_x &&
                   peanut->x_player <= peanut->clouds[i]->clouds_x +
                                           peanut->clouds[i]->width)) {
                if (peanut->collision_player == 0) {
                  if (peanut->life_player == 1)
                    changeState(GAME_COLLISION, state);
                  else {
                    peanut->collision_player = 1;
                    peanut->collision_cloud = i;
                    peanut->life_player--;
                  }
                }
              }

              break;
          }
      }
    }
  }
  if (peanut->out_boundaries == 0) {
    if (peanut->y_player <= 0 || peanut->y_player >= V_RES) {
      if (peanut->life_player == 1)
        changeState(GAME_COLLISION, state);
      else {
        peanut->life_player--;
        peanut->out_boundaries = 1;
      }
    }
  }
}

void game_score(JumpingPeanut* peanut) {
  int i;

  for (i = 0; i < 100; ++i) {
    if (peanut->clouds[i] != NULL) {
      if (peanut->x_player ==
          peanut->clouds[i]->clouds_x + peanut->clouds[i]->width) {
        peanut->score_player++;
      }
    }
  }
}

void game_draw_score(JumpingPeanut* peanut) {
  int st, s1, s2, s3;

  if (peanut->score_player < 10) {
    game_switch_numbers(peanut->score_player, 497, 100, peanut);
  }

  else if (peanut->score_player >= 10 && peanut->score_player < 100) {
    st = peanut->score_player;

    s1 = st / 10;

    s2 = st % 10;

    game_switch_numbers(s1, 482, 100, peanut);
    game_switch_numbers(s2, 503, 100, peanut);
  }

  else if(peanut->score_player >= 100 && peanut->score_player < 1000) {

    st = peanut->score_player;

    s1 = (st / 100);

    s2 = (st / 10) % 10;

    s3 = st % 10;

    game_switch_numbers(s1, 467, 100, peanut);
    game_switch_numbers(s2, 488, 100, peanut);
    game_switch_numbers(s3, 509, 100, peanut);

  }

}

void game_draw_life(JumpingPeanut* peanut) {
  if (peanut->life_player == 3) {
    bitmapDrawer(peanut->heart, 452, 125);
    bitmapDrawer(peanut->heart, 490, 125);
    bitmapDrawer(peanut->heart, 528, 125);
  }

  if (peanut->life_player == 2) {
    bitmapDrawer(peanut->heart, 472, 125);
    bitmapDrawer(peanut->heart, 510, 125);
  }

  if (peanut->life_player == 1) {
    bitmapDrawer(peanut->heart, 492, 125);
  }
}

void game_draw_time(JumpingPeanut* peanut, Time* timer){

  getCurrentTime(timer);

  int ht, h1, h2, mt, m1, m2, st, s1, s2;

  ht = timer->hour;
  mt = timer->minute;
  st = timer->second;

  h1 = ht / 10;
  h2 = ht % 10;
  m1 = mt / 10;
  m2 = mt % 10;
  s1 = st / 10;
  s2 = st % 10;

  game_switch_numbers(h1, 844, 738, peanut);
  game_switch_numbers(h2, 865, 738, peanut);
  bitmapDrawer(peanut->points, 886,738);
  game_switch_numbers(m1, 907, 738, peanut);
  game_switch_numbers(m2, 928, 738, peanut);
  bitmapDrawer(peanut->points, 949, 738);
  game_switch_numbers(s1, 970, 738, peanut);
  game_switch_numbers(s2, 991, 738, peanut);

}

void game_switch_numbers(int option, int x, int y, JumpingPeanut* peanut) {
  switch (option) {
    case 0:
      bitmapDrawer(peanut->zero, x, y);
      break;
    case 1:
      bitmapDrawer(peanut->one, x, y);
      break;
    case 2:
      bitmapDrawer(peanut->two, x, y);
      break;
    case 3:
      bitmapDrawer(peanut->three, x, y);
      break;
    case 4:
      bitmapDrawer(peanut->four, x, y);
      break;
    case 5:
      bitmapDrawer(peanut->five, x, y);
      break;
    case 6:
      bitmapDrawer(peanut->six, x, y);
      break;
    case 7:
      bitmapDrawer(peanut->seven, x, y);
      break;
    case 8:
      bitmapDrawer(peanut->eigth, x, y);
      break;
    case 9:
      bitmapDrawer(peanut->nine, x, y);
      break;
  }
}

void game_rtc(JumpingPeanut* peanut, Time* timer) {

  if(timer->hour >= 8 && timer->hour < 19){

    peanut->background = loadBitmap("/home/lcom/proj/res/background.bmp");
    peanut->image_1 = loadBitmap("/home/lcom/proj/res/peanut_p1.bmp");
    peanut->image_2 = loadBitmap("/home/lcom/proj/res/peanut_p2.bmp");
  }
  else if(timer->hour < 8){

    peanut->background = loadBitmap("/home/lcom/proj/res/background_night.bmp");
    peanut->image_1 = loadBitmap("/home/lcom/proj/res/peanut_p1_night.bmp");
    peanut->image_2 = loadBitmap("/home/lcom/proj/res/peanut_p2_night.bmp");
  }
  else if(timer->hour >= 19){
    peanut->background = loadBitmap("/home/lcom/proj/res/background_night.bmp");
    peanut->image_1 = loadBitmap("/home/lcom/proj/res/peanut_p1_night.bmp");
    peanut->image_2 = loadBitmap("/home/lcom/proj/res/peanut_p2_night.bmp");
  }
}

void game_try_again(JumpingPeanut* peanut) {

  int i;

  peanut->x_player = PLAYER_START_X;
  peanut->y_player = PLAYER_START_Y;
  peanut->new_y_player = PLAYER_START_Y;
  peanut->jump_player = 0;
  peanut->score_player = 0;
  peanut->life_player = 3;
  peanut->collision_player = 0;
  peanut->out_boundaries = 0;

  for (i = 0; i < 100; ++i) {
    if (peanut->clouds[i] != NULL) {
        deleteClouds(peanut->clouds[i]);
        peanut->clouds[i] = NULL;
    }
  }
}

void game_delete_bitmaps(JumpingPeanut* peanut) {

  deleteBitmap(peanut->menu);
  deleteBitmap(peanut->gameOver);
  deleteBitmap(peanut->heart);
  deleteBitmap(peanut->zero);
  deleteBitmap(peanut->one);
  deleteBitmap(peanut->two);
  deleteBitmap(peanut->three);
  deleteBitmap(peanut->four);
  deleteBitmap(peanut->five);
  deleteBitmap(peanut->six);
  deleteBitmap(peanut->seven);
  deleteBitmap(peanut->eigth);
  deleteBitmap(peanut->nine);
  deleteBitmap(peanut->background);
  deleteBitmap(peanut->image_1);
  deleteBitmap(peanut->image_2);
  deleteBitmap(get_mouse()->icon);
}
