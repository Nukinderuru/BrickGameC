#ifndef SRC_BRICK_GAME_TETRIS_TETRIS_BOARD_H_
#define SRC_BRICK_GAME_TETRIS_TETRIS_BOARD_H_

#include <stdbool.h>

#include "tetris_internal.h"

bool tetrisCheckCollision(const TetrisGame_t *game, const Tetromino_t *piece,
                          int row, int col);
bool tetrisCanPlace(const TetrisGame_t *game, const Tetromino_t *piece);
void tetrisMergeCurrent(TetrisGame_t *game);
void tetrisClearBoard(TetrisGame_t *game);
int tetrisClearLines(TetrisGame_t *game);
bool tetrisTopReached(const TetrisGame_t *game);
void tetrisRebuildPreview(TetrisGame_t *game);

#endif  // SRC_BRICK_GAME_TETRIS_TETRIS_BOARD_H_
