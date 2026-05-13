#include "tetris_board.h"

#include <string.h>

bool tetrisCheckCollision(const TetrisGame_t *game, const Tetromino_t *piece,
                          const int row, const int col) {
  bool collision = false;
  for (int r = 0; r < TETROMINO_SIZE && !collision; ++r) {
    for (int c = 0; c < TETROMINO_SIZE && !collision; ++c) {
      if (!piece->cells[r][c]) {
        continue;
      }
      const int board_row = row + r;
      const int board_col = col + c;
      if (board_row < 0 || board_row >= TETRIS_ROWS || board_col < 0 ||
          board_col >= TETRIS_COLS || game->board[board_row][board_col]) {
        collision = true;
      }
    }
  }
  return collision;
}

bool tetrisCanPlace(const TetrisGame_t *game, const Tetromino_t *piece) {
  return !tetrisCheckCollision(game, piece, piece->row, piece->col);
}

void tetrisMergeCurrent(TetrisGame_t *game) {
  const int value = game->current.type + 1;
  for (int r = 0; r < TETROMINO_SIZE; ++r) {
    for (int c = 0; c < TETROMINO_SIZE; ++c) {
      if (game->current.cells[r][c]) {
        const int board_row = game->current.row + r;
        const int board_col = game->current.col + c;
        if (board_row >= 0 && board_row < TETRIS_ROWS && board_col >= 0 &&
            board_col < TETRIS_COLS) {
          game->board[board_row][board_col] = value;
        }
      }
    }
  }
}

void tetrisClearBoard(TetrisGame_t *game) {
  memset(game->board, 0, sizeof(game->board));
}

int tetrisClearLines(TetrisGame_t *game) {
  int cleared = 0;
  for (int row = TETRIS_ROWS - 1; row >= 0; --row) {
    bool full = true;
    for (int col = 0; col < TETRIS_COLS; ++col) {
      if (!game->board[row][col]) {
        full = false;
      }
    }
    if (full) {
      ++cleared;
      for (int move_row = row; move_row > 0; --move_row) {
        memcpy(game->board[move_row], game->board[move_row - 1],
               sizeof(game->board[move_row]));
      }
      memset(game->board[0], 0, sizeof(game->board[0]));
      ++row;
    }
  }
  return cleared;
}

bool tetrisTopReached(const TetrisGame_t *game) {
  bool reached = false;
  for (int col = 0; col < TETRIS_COLS && !reached; ++col) {
    if (game->board[0][col]) {
      reached = true;
    }
  }
  return reached;
}

void tetrisRebuildPreview(TetrisGame_t *game) {
  memset(game->next_storage, 0, sizeof(game->next_storage));
  for (int r = 0; r < TETROMINO_SIZE; ++r) {
    for (int c = 0; c < TETROMINO_SIZE; ++c) {
      game->next_storage[r][c] =
          game->next_piece.cells[r][c] ? game->next_piece.type + 1 : 0;
    }
  }
}
