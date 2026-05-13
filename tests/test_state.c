#include <check.h>

#include <string.h>

#include "../src/brick_game/tetris/tetris_internal.h"

static void set_single_cell_piece(Tetromino_t *piece) {
  memset(piece, 0, sizeof(*piece));
  piece->cells[0][0] = 1;
  piece->row = 0;
  piece->col = 3;
}

static void reset_to_start(void) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisReset(game);
  game->state = kStateStart;
  game->paused = false;
  game->game_over = false;
  game->running = false;
  game->exit_requested = false;
  game->has_current = false;
}

START_TEST(start_transitions_to_moving) {
  reset_to_start();
  userInput(Start, false);
  updateCurrentState();
  ck_assert_int_eq(tetrisGetGame()->state, kStateMoving);
  ck_assert_int_eq(tetrisGetGame()->has_current, true);
}
END_TEST

START_TEST(pause_toggles_in_moving_state) {
  reset_to_start();
  userInput(Start, false);
  updateCurrentState();
  userInput(Pause, false);
  ck_assert_int_eq(tetrisGetGame()->paused, true);
  userInput(Pause, false);
  ck_assert_int_eq(tetrisGetGame()->paused, false);
}
END_TEST

START_TEST(terminate_sets_game_over) {
  reset_to_start();
  userInput(Terminate, false);
  ck_assert_int_eq(tetrisGetGame()->state, kStateGameOver);
  ck_assert_int_eq(tetrisGetGame()->exit_requested, true);
}
END_TEST

START_TEST(spawn_collision_enters_game_over) {
  reset_to_start();
  TetrisGame_t *game = tetrisGetGame();
  game->running = true;
  game->state = kStateSpawn;
  game->has_current = false;
  set_single_cell_piece(&game->next_piece);
  game->board[0][3] = 1;
  updateCurrentState();
  ck_assert_int_eq(game->state, kStateGameOver);
  ck_assert_int_eq(game->game_over, true);
}
END_TEST

START_TEST(game_over_start_restarts_game) {
  reset_to_start();
  TetrisGame_t *game = tetrisGetGame();
  game->state = kStateGameOver;
  game->game_over = true;
  game->exit_requested = true;
  userInput(Start, false);
  updateCurrentState();
  ck_assert_int_eq(game->state, kStateMoving);
  ck_assert_int_eq(game->game_over, false);
  ck_assert_int_eq(game->exit_requested, false);
}
END_TEST

Suite *tetris_state_suite(void) {
  Suite *suite = suite_create("state");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, start_transitions_to_moving);
  tcase_add_test(tcase, pause_toggles_in_moving_state);
  tcase_add_test(tcase, terminate_sets_game_over);
  tcase_add_test(tcase, spawn_collision_enters_game_over);
  tcase_add_test(tcase, game_over_start_restarts_game);
  suite_add_tcase(suite, tcase);
  return suite;
}
