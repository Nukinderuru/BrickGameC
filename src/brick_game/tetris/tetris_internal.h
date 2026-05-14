#ifndef SRC_BRICK_GAME_TETRIS_TETRIS_INTERNAL_H_
#define SRC_BRICK_GAME_TETRIS_TETRIS_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

#include "tetris.h"

#define TETRIS_ROWS 20
#define TETRIS_COLS 10
#define TETRIS_PREVIEW_SIZE 4
#define TETROMINO_SIZE 4
#define TETROMINO_COUNT 7
#define ROTATION_STATES 4
#define TETRIS_MAX_LEVEL 10

typedef enum {
  kStateStart = 0,
  kStateSpawn,
  kStateMoving,
  kStateMove,
  kStateAttaching,
  kStateGameOver
} TetrisState_t;

typedef struct {
  int cells[TETROMINO_SIZE][TETROMINO_SIZE];
  int type;
  int rotation;
  int row;
  int col;
} Tetromino_t;

typedef struct {
  int board[TETRIS_ROWS][TETRIS_COLS];
  int field_storage[TETRIS_ROWS][TETRIS_COLS];
  int next_storage[TETRIS_PREVIEW_SIZE][TETRIS_PREVIEW_SIZE];
  int *field_rows[TETRIS_ROWS];
  int *next_rows[TETRIS_PREVIEW_SIZE];

  Tetromino_t current;
  Tetromino_t next_piece;

  TetrisState_t state;
  bool initialized;
  bool running;
  bool paused;
  bool game_over;
  bool exit_requested;
  bool has_current;
  bool soft_drop;

  uint64_t last_tick_ms;
  uint64_t base_fall_delay_ms;
  uint64_t soft_drop_delay_ms;

  int score;
  int high_score;
  int level;
  int speed;

  GameInfo_t info;
} TetrisGame_t;

TetrisGame_t *tetrisGetGame(void);
void tetrisEnsureInitialized(void);
void tetrisReset(TetrisGame_t *game);
void tetrisSyncInfo(TetrisGame_t *game);
uint64_t tetrisNowMs(void);
void tetrisResetSingletonForTests(void);

#endif  // SRC_BRICK_GAME_TETRIS_TETRIS_INTERNAL_H_
