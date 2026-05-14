#include "tetris_logic.h"

#include "tetris_board.h"
#include "tetris_random.h"
#include "tetris_storage.h"

static uint64_t tetrisDelayForLevel(const int level) {
  uint64_t delay = 650ULL;
  if (level > 1) {
    delay -= (uint64_t)(level - 1) * 55ULL;
  }
  if (delay < 155ULL) {
    delay = 155ULL;
  }
  return delay;
}

static void tetrisUpdateProgression(TetrisGame_t *game) {
  int level = game->score / 600 + 1;
  if (level > TETRIS_MAX_LEVEL) {
    level = TETRIS_MAX_LEVEL;
  }
  game->level = level;
  game->speed = level;
  game->base_fall_delay_ms = tetrisDelayForLevel(level);
}

static int tetrisScoreForLines(const int cleared_lines) {
  int score = 0;
  switch (cleared_lines) {
    case 1:
      score = 100;
      break;
    case 2:
      score = 300;
      break;
    case 3:
      score = 700;
      break;
    case 4:
      score = 1500;
      break;
    default:
      break;
  }
  return score;
}

static void tetrisApplyScore(TetrisGame_t *game, const int cleared_lines) {
  game->score += tetrisScoreForLines(cleared_lines);
  tetrisUpdateProgression(game);
  if (game->score > game->high_score) {
    game->high_score = game->score;
    tetrisSaveHighScore(game->high_score);
  }
}

void tetrisSpawnNextPiece(TetrisGame_t *game) {
  game->current = game->next_piece;
  game->current.row = 0;
  game->current.col = 3;
  game->current.rotation = 0;
  game->next_piece = tetrisRandomPiece();
  game->has_current = true;
  tetrisRebuildPreview(game);
}

bool tetrisMoveCurrent(TetrisGame_t *game, const int drow, const int dcol) {
  bool moved = false;
  if (!game->has_current) {
    return moved;
  }
  const int next_row = game->current.row + drow;
  const int next_col = game->current.col + dcol;
  if (!tetrisCheckCollision(game, &game->current, next_row, next_col)) {
    game->current.row = next_row;
    game->current.col = next_col;
    moved = true;
  }
  return moved;
}

bool tetrisRotateCurrent(TetrisGame_t *game) {
  bool rotated = false;
  if (!game->has_current) {
    return rotated;
  }
  Tetromino_t candidate = game->current;
  for (int r = 0; r < TETROMINO_SIZE; ++r) {
    for (int c = 0; c < TETROMINO_SIZE; ++c) {
      candidate.cells[c][TETROMINO_SIZE - 1 - r] = game->current.cells[r][c];
    }
  }
  if (!tetrisCheckCollision(game, &candidate, candidate.row, candidate.col)) {
    candidate.rotation = (candidate.rotation + 1) % ROTATION_STATES;
    game->current = candidate;
    rotated = true;
  }
  return rotated;
}

void tetrisLockCurrent(TetrisGame_t *game) {
  if (!game->has_current) {
    return;
  }
  tetrisMergeCurrent(game);
  game->has_current = false;
  tetrisApplyScore(game, tetrisClearLines(game));
}
