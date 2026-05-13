#ifndef SRC_BRICK_GAME_TETRIS_TETRIS_LOGIC_H_
#define SRC_BRICK_GAME_TETRIS_TETRIS_LOGIC_H_

#include <stdbool.h>

#include "tetris_internal.h"

void tetrisSpawnNextPiece(TetrisGame_t *game);
bool tetrisMoveCurrent(TetrisGame_t *game, int drow, int dcol);
bool tetrisRotateCurrent(TetrisGame_t *game);
void tetrisLockCurrent(TetrisGame_t *game);

#endif  // SRC_BRICK_GAME_TETRIS_TETRIS_LOGIC_H_
