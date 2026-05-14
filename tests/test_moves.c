#include <check.h>

#include <string.h>

#include "../src/brick_game/tetris/tetris_internal.h"

static void set_single_cell_piece(TetrisGame_t *game) {
  memset(game->current.cells, 0, sizeof(game->current.cells));
  game->current.cells[0][0] = 1;
  game->current.row = 0;
  game->current.col = 3;
  game->current.rotation = 0;
  game->has_current = true;
}

static void setup_game(void) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisReset(game);
  game->state = kStateStart;
  userInput(Start, false);
  updateCurrentState();
}

START_TEST(piece_moves_left_and_right) {
  setup_game();
  const int original = tetrisGetGame()->current.col;
  userInput(Left, false);
  ck_assert_int_eq(tetrisGetGame()->current.col, original - 1);
  userInput(Right, false);
  ck_assert_int_eq(tetrisGetGame()->current.col, original);
}
END_TEST

START_TEST(piece_stops_at_left_wall) {
  setup_game();
  TetrisGame_t *game = tetrisGetGame();
  set_single_cell_piece(game);
  game->current.col = 0;
  userInput(Left, false);
  ck_assert_int_eq(game->current.col, 0);
}
END_TEST

START_TEST(down_tap_moves_piece_one_row) {
  setup_game();
  const int original = tetrisGetGame()->current.row;
  userInput(Down, false);
  ck_assert_int_eq(tetrisGetGame()->current.row, original + 1);
}
END_TEST

START_TEST(paused_game_blocks_horizontal_input) {
  setup_game();
  const TetrisGame_t *game = tetrisGetGame();
  const int original = game->current.col;
  userInput(Pause, false);
  userInput(Left, false);
  ck_assert_int_eq(game->current.col, original);
}
END_TEST

START_TEST(paused_game_blocks_gravity_tick) {
  setup_game();
  TetrisGame_t *game = tetrisGetGame();
  set_single_cell_piece(game);
  const int original = game->current.row;
  userInput(Pause, false);
  game->last_tick_ms = 0;
  updateCurrentState();
  ck_assert_int_eq(game->current.row, original);
  ck_assert_int_eq(game->state, kStateMoving);
}
END_TEST

START_TEST(soft_drop_sets_flag_without_changing_level_speed) {
  setup_game();
  userInput(Down, true);
  const GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(tetrisGetGame()->soft_drop, true);
  ck_assert_int_eq(info.speed, 1);
}
END_TEST

START_TEST(soft_drop_timer_moves_piece_down) {
  setup_game();
  TetrisGame_t *game = tetrisGetGame();
  set_single_cell_piece(game);
  const int original = game->current.row;
  userInput(Down, true);
  game->last_tick_ms = 0;
  updateCurrentState();
  ck_assert_int_eq(game->current.row, original + 1);
}
END_TEST

START_TEST(soft_drop_release_restores_normal_level_speed) {
  setup_game();
  userInput(Down, true);
  updateCurrentState();
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.speed, 1);
  userInput(Down, false);
  info = updateCurrentState();
  ck_assert_int_eq(tetrisGetGame()->soft_drop, false);
  ck_assert_int_eq(info.speed, 1);
}
END_TEST

START_TEST(soft_drop_release_does_not_add_extra_step) {
  setup_game();
  TetrisGame_t *game = tetrisGetGame();
  set_single_cell_piece(game);
  userInput(Down, true);
  game->last_tick_ms = 0;
  updateCurrentState();
  const int row_after_soft_drop = game->current.row;
  userInput(Down, false);
  ck_assert_int_eq(game->soft_drop, false);
  ck_assert_int_eq(game->current.row, row_after_soft_drop);
}
END_TEST

Suite *tetris_move_suite(void) {
  Suite *suite = suite_create("moves");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, piece_moves_left_and_right);
  tcase_add_test(tcase, piece_stops_at_left_wall);
  tcase_add_test(tcase, down_tap_moves_piece_one_row);
  tcase_add_test(tcase, paused_game_blocks_horizontal_input);
  tcase_add_test(tcase, paused_game_blocks_gravity_tick);
  tcase_add_test(tcase, soft_drop_sets_flag_without_changing_level_speed);
  tcase_add_test(tcase, soft_drop_timer_moves_piece_down);
  tcase_add_test(tcase, soft_drop_release_restores_normal_level_speed);
  tcase_add_test(tcase, soft_drop_release_does_not_add_extra_step);
  suite_add_tcase(suite, tcase);
  return suite;
}
