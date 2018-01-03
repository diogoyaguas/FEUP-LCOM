#include <stdlib.h>
#include "stateMachine.h"
#include "jumpingPeanut.h"
#include "i8042.h"
#include "bitmap.h"
#include "video_gr.h"

State* setState() {
  State* gameState = (State*)malloc(sizeof(State));

  gameState->currentState = MENU;

  return gameState;
}

void changeState(transitionState transition, State* State) {
  switch (State->currentState) {
    case MENU:
      if (transition == GAME_PLAY)
        State->currentState = GAME;
      else if (transition == GAME_ESC) {
        State->currentState = GAME_CLOSE;
      }
      break;

    case GAME:
      if (transition == GAME_ESC)
        State->currentState = GAME_CLOSE;
      else if (transition == GAME_COLLISION)
        State->currentState = GAMEOVER;

      break;

    case GAMEOVER:
      if (transition == GAME_PLAY)
        State->currentState = GAME;
      else if (transition == GAME_ESC)
        State->currentState = GAME_CLOSE;
      break;

    default:

      break;
  }
}

void deleteState(State* gameState) { free(gameState); }
