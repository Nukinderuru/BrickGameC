#include <check.h>

#include <string.h>

#include "../src/brick_game/tetris/tetris_internal.h"

static void set_t_piece(TetrisGame_t *game) {
  static const int kTPiece[TETROMINO_SIZE][TETROMINO_SIZE] = {
      {0, 1, 0, 0},
      {1, 1, 1, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
  };
  memset(game->current.cells, 0, sizeof(game->current.cells));
  for (int row = 0; row < TETROMINO_SIZE; ++row) {
    for (int col = 0; col < TETROMINO_SIZE; ++col) {
      game->current.cells[row][col] = kTPiece[row][col];
    }
  }
  game->current.type = 2;
  game->current.rotation = 0;
}

static void setup_rotation_game(void) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisReset(game);
  game->state = kStateStart;
  userInput(Start, false);
  updateCurrentState();
}

START_TEST(action_rotates_piece_when_possible) {
  setup_rotation_game();
  TetrisGame_t *game = tetrisGetGame();
  set_t_piece(game);
  game->current.col = 3;
  const int before = game->current.rotation;
  userInput(Action, false);
  ck_assert_int_ne(game->current.rotation, before);
}
END_TEST

START_TEST(rotation_can_be_blocked_by_collision) {
  setup_rotation_game();
  TetrisGame_t *game = tetrisGetGame();
  set_t_piece(game);
  game->current.row = 0;
  game->current.col = 3;
  game->board[1][5] = 1;
  const int before = game->current.rotation;
  userInput(Action, false);
  ck_assert_int_eq(game->current.rotation, before);
}
END_TEST

Suite *tetris_rotation_suite(void) {
  Suite *suite = suite_create("rotation");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, action_rotates_piece_when_possible);
  tcase_add_test(tcase, rotation_can_be_blocked_by_collision);
  suite_add_tcase(suite, tcase);
  return suite;
}
