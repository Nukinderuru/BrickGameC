#include "tetris_fsm.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tetris_board.h"
#include "tetris_logic.h"
#include "tetris_random.h"
#include "tetris_storage.h"

static void tetrisStartGame(TetrisGame_t *game) {
  tetrisReset(game);
  game->running = true;
  game->paused = false;
  game->game_over = false;
  game->exit_requested = false;
  game->state = kStateSpawn;
  game->last_tick_ms = tetrisNowMs();
}

static void tetrisHandleStartState(TetrisGame_t *game, const UserAction_t action) {
  if (action == Start) {
    tetrisStartGame(game);
  } else if (action == Terminate) {
    game->game_over = true;
    game->exit_requested = true;
    game->state = kStateGameOver;
  }
}

static void tetrisHandleMovingState(TetrisGame_t *game, const UserAction_t action,
                                    const bool hold) {
  if (action == Pause) {
    game->paused = !game->paused;
    return;
  }
  if (action == Terminate) {
    game->game_over = true;
    game->exit_requested = true;
    game->state = kStateGameOver;
    return;
  }
  if (game->paused) {
    return;
  }
  switch (action) {
    case Left:
      tetrisMoveCurrent(game, 0, -1);
      break;
    case Right:
      tetrisMoveCurrent(game, 0, 1);
      break;
    case Action:
      tetrisRotateCurrent(game);
      break;
    case Down:
      if (hold) {
        game->soft_drop = true;
      } else if (game->soft_drop) {
        game->soft_drop = false;
      } else {
        tetrisMoveCurrent(game, 1, 0);
      }
      break;
    default:
      break;
  }
}

void tetrisHandleInput(const UserAction_t action, const bool hold) {
  TetrisGame_t *game = tetrisGetGame();
  switch (game->state) {
    case kStateStart:
      tetrisHandleStartState(game, action);
      break;
    case kStateSpawn:
    case kStateMove:
    case kStateAttaching:
      if (action == Terminate) {
        game->game_over = true;
        game->exit_requested = true;
        game->state = kStateGameOver;
      }
      break;
    case kStateMoving:
      tetrisHandleMovingState(game, action, hold);
      break;
    case kStateGameOver:
      if (action == Start) {
        tetrisStartGame(game);
      } else if (action == Terminate) {
        game->exit_requested = true;
      }
      break;
  }
}

static void tetrisUpdateSpawn(TetrisGame_t *game) {
  tetrisSpawnNextPiece(game);
  if (tetrisCanPlace(game, &game->current)) {
    game->state = kStateMoving;
  } else {
    game->game_over = true;
    game->state = kStateGameOver;
  }
}

static void tetrisUpdateMove(TetrisGame_t *game) {
  if (tetrisMoveCurrent(game, 1, 0)) {
    game->state = kStateMoving;
  } else {
    game->state = kStateAttaching;
  }
}

static void tetrisUpdateAttaching(TetrisGame_t *game) {
  tetrisLockCurrent(game);
  if (tetrisTopReached(game)) {
    game->game_over = true;
    game->state = kStateGameOver;
  } else {
    game->state = kStateSpawn;
  }
}

void tetrisUpdate(TetrisGame_t *game) {
  if (game->state == kStateSpawn) {
    tetrisUpdateSpawn(game);
  }

  if (game->state == kStateMove) {
    tetrisUpdateMove(game);
  }

  if (game->state == kStateAttaching) {
    tetrisUpdateAttaching(game);
  }

  if (game->state == kStateMoving && !game->paused && game->running) {
    const uint64_t now = tetrisNowMs();
    const uint64_t delay = game->soft_drop ? game->soft_drop_delay_ms
                                     : game->base_fall_delay_ms;
    if (now - game->last_tick_ms >= delay) {
      game->last_tick_ms = now;
      if (tetrisCheckCollision(game, &game->current, game->current.row + 1,
                               game->current.col)) {
        game->state = kStateAttaching;
      } else {
        game->state = kStateMove;
      }
    }
  }

  if (game->state == kStateMove) {
    tetrisUpdateMove(game);
  }

  if (game->state == kStateAttaching) {
    tetrisUpdateAttaching(game);
  }
}

TetrisGame_t *tetrisGetGame(void) {
  static TetrisGame_t game;
  return &game;
}

void tetrisReset(TetrisGame_t *game) {
  tetrisClearBoard(game);
  game->current = (Tetromino_t){0};
  game->next_piece = tetrisRandomPiece();
  game->has_current = false;
  game->soft_drop = false;
  game->score = 0;
  game->level = 1;
  game->speed = 1;
  game->base_fall_delay_ms = 650ULL;
  game->info.pause = 0;
  tetrisRebuildPreview(game);
}

void tetrisEnsureInitialized(void) {
  TetrisGame_t *game = tetrisGetGame();
  if (game->initialized) {
    return;
  }
  srand((unsigned int)time(NULL));
  for (int row = 0; row < TETRIS_ROWS; ++row) {
    game->field_rows[row] = game->field_storage[row];
  }
  for (int row = 0; row < TETRIS_PREVIEW_SIZE; ++row) {
    game->next_rows[row] = game->next_storage[row];
  }
  game->info.field = game->field_rows;
  game->info.next = game->next_rows;
  game->base_fall_delay_ms = 650ULL;
  game->soft_drop_delay_ms = 75;
  game->state = kStateStart;
  game->high_score = tetrisLoadHighScore();
  tetrisReset(game);
  game->initialized = true;
  tetrisSyncInfo(game);
}

uint64_t tetrisNowMs(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
}

void tetrisSyncInfo(TetrisGame_t *game) {
  for (int row = 0; row < TETRIS_ROWS; ++row) {
    for (int col = 0; col < TETRIS_COLS; ++col) {
      game->field_storage[row][col] = game->board[row][col];
    }
  }
  if (game->has_current && game->state != kStateGameOver) {
    const int value = game->current.type + 1;
    for (int r = 0; r < TETROMINO_SIZE; ++r) {
      for (int c = 0; c < TETROMINO_SIZE; ++c) {
        if (game->current.cells[r][c]) {
          const int board_row = game->current.row + r;
          const int board_col = game->current.col + c;
          if (board_row >= 0 && board_row < TETRIS_ROWS && board_col >= 0 &&
              board_col < TETRIS_COLS) {
            game->field_storage[board_row][board_col] = value;
          }
        }
      }
    }
  }
  game->info.score = game->score;
  game->info.high_score = game->high_score;
  game->info.level = game->level;
  game->info.speed = game->speed;
  game->info.pause = game->paused ? 1 : 0;
}

void tetrisResetSingletonForTests(void) {
  TetrisGame_t *game = tetrisGetGame();
  memset(game, 0, sizeof(*game));
}
