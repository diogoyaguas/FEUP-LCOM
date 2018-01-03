#ifndef STATEMACHINE_H
#define STATEMACHINE_H

/** @defgroup stateMachine stateMachine
 * @{
 *
 * Functions related with state machine
 */

#include "bitmap.h"

typedef enum { MENU, GAME, GAMEOVER, GAME_CLOSE } gametState;

typedef enum {
  GAME_PLAY,
  GAME_COLLISION,
  GAME_ESC,
} transitionState;

typedef struct { gametState currentState; } State;

/**
 * @brief Initializes the State structure.
 */
State* setState();

/**
 * @brief Updates the state related to the menus and the game.
 * @param transition
 * @param State machine
 */
void changeState(transitionState transition, State* gameState);

/**
 * @brief Deletes the state.
 * @param State machine
 */
void deleteState(State* gameState);

/** @} end of stateMachine */

#endif /* __STATEMACHINE_H */
