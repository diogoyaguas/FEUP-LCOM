#ifndef _GAME_H
#define _GAME_H

/** @defgroup game game
 * @{
 *
 * Function which assist the role-playong of the game
 */

#include "jumpingPeanut.h"
#include "stateMachine.h"
#include "rtc.h"

/**
 * @brief Handles what is related to the keyboard interrupts.
 * @param Jumping Peanut
 * @param State machine
 */
void game_kbd(JumpingPeanut* peanut, State* state);

/**
 * @brief Handles what is related to the main menu.
 * @param Jumping Peanut
 * @param State machine
 */
void game_menu(JumpingPeanut* peanut, State* state);

/**
 * @brief coordinates for the main menu options.
 * @param State machine
 */
void game_menu_buttons(State* state);

/**
 * @brief Handles what is related to the gameover menu.
 * @param Jumping Peanut
 * @param State machine
 */
void game_gameOverMenu(JumpingPeanut* peanut, State* state);

/**
 * @brief coordinates for the game over menu options.
 * @param Jumping Peanut
 * @param State machine
 */
void game_gameOver_buttons(State* state, JumpingPeanut* peanut);

/**
 * @brief Handles the jumping and the fall of the character.
 * @param Jumping Peanut
 * @param State machine
 */
void game_vertical_move(JumpingPeanut* peanut, Time* timer);

/**
 * @brief Draws the differents sets of clouds.
 * @param Jumping Peanut
 */
void game_clouds(JumpingPeanut* peanut);

/**
 * @brief Conditions for the collisions happen.
 * @param Jumping Peanut
 * @param State machine
 */
void game_collision(JumpingPeanut* peanut, State* state);

/**
 * @brief Conditions to increment the score.
 * @param Jumping Peanut
 */
void game_score(JumpingPeanut* peanut);

/**
 * @brief Draws the score.
 * @param Jumping Peanut
 */
void game_draw_score(JumpingPeanut* peanut);

/**
 * @brief Draws the hearts related to the lives the player has.
 * @param Jumping Peanut
 */
void game_draw_life(JumpingPeanut* peanut);

/**
 * @brief Draws the time that comes from the rtc.
 * @param Jumping Peanut
 * @param Time
 */
void game_draw_time(JumpingPeanut* peanut, Time* timer);

/**
 * @brief Numbers for the score.
 * @param Number of the score
 * @param Destiny x coord
 * @param Destiny y coord
 * @param Jumping Peanut
 */
void game_switch_numbers(int option, int x, int y, JumpingPeanut* peanut);

/**
 * @brief Changes the background of the game according to the hours.
 * @param Jumping Peanut
 * @param Time
 */
void game_rtc(JumpingPeanut* peanut, Time* timer);

/**
 * @brief Handles what is needed to restastart the game in the game over menu.
 * @param Jumping Peanut
 */
void game_try_again(JumpingPeanut* peanut);

/**
 * @brief Deletes all the bitmaps.
 * @param Jumping Peanut
 */
void game_delete_bitmaps(JumpingPeanut* peanut);

/** @} end of game */

#endif /*  _GAME_H  */
