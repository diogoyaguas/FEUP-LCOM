#include <minix/drivers.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "i8254.h"
#include "i8042.h"

#include "vbe.h"
#include "video_gr.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "rtc.h"
#include "game.h"
#include "jumpingPeanut.h"
#include "stateMachine.h"

int main() {
  sef_startup();
  sys_enable_iop(SELF);

  srand(time(NULL));

  vg_init(0x117);

  JumpingPeanut* peanut = (JumpingPeanut*)startJumpingPeanut();
  State* state = (State*)setState();
  Time* timer = (Time*)initTime();
  getCurrentTime(timer);
  game_rtc(peanut, timer);
  printf("%lu\n", timer->hour);

  while (!peanut->done) {
    if (state->currentState == MENU) {
      game_menu(peanut, state);
    }

    else if (state->currentState == GAME) {
      updateJumpingPeanut(peanut, state, timer);
    }

    else if (state->currentState == GAMEOVER) {
      game_gameOverMenu(peanut, state);
    }

    else if (state->currentState == GAME_CLOSE) {
      peanut->done = 1;
    }
  }

  stopJumpingPeanut(peanut);
  deleteState(state);
  delete_mouse();

  if (vg_exit() != 0) {
    return -1;
  }

  return 0;
}
