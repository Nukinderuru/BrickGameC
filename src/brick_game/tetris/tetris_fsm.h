#ifndef SRC_BRICK_GAME_TETRIS_TETRIS_FSM_H_
#define SRC_BRICK_GAME_TETRIS_TETRIS_FSM_H_

#include <stdbool.h>

#include "tetris_internal.h"

void tetrisHandleInput(UserAction_t action, bool hold);
void tetrisUpdate(TetrisGame_t *game);

#endif  // SRC_BRICK_GAME_TETRIS_TETRIS_FSM_H_
