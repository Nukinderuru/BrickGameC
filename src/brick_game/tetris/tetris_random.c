#include "tetris_random.h"

#include <stdlib.h>

static const int kTetrominoShapes[TETROMINO_COUNT][TETROMINO_SIZE]
                                 [TETROMINO_SIZE] = {
    {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
};

Tetromino_t tetrisRandomPiece(void) {
  Tetromino_t piece = {0};
  piece.type = rand() % TETROMINO_COUNT;
  piece.rotation = 0;
  piece.row = 0;
  piece.col = 3;
  for (int r = 0; r < TETROMINO_SIZE; ++r) {
    for (int c = 0; c < TETROMINO_SIZE; ++c) {
      piece.cells[r][c] = kTetrominoShapes[piece.type][r][c];
    }
  }
  return piece;
}
