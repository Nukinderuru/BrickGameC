#include <check.h>

#include <string.h>

#include "../src/brick_game/tetris/tetris_internal.h"
#include "../src/brick_game/tetris/tetris_logic.h"

static void reset_level_game(void) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->high_score = 999999;
}

static void make_vertical_piece(TetrisGame_t *game, const int height,
                                const int row, const int col) {
  memset(game->current.cells, 0, sizeof(game->current.cells));
  for (int i = 0; i < height; ++i) {
    game->current.cells[i][0] = 1;
  }
  game->current.type = 0;
  game->current.row = row;
  game->current.col = col;
  game->has_current = true;
}

START_TEST(initial_level_and_speed_are_one) {
  reset_level_game();
  TetrisGame_t *game = tetrisGetGame();
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(game->level, 1);
  ck_assert_int_eq(game->speed, 1);
  ck_assert_uint_eq(game->base_fall_delay_ms, 650ULL);
  ck_assert_int_eq(info.level, 1);
  ck_assert_int_eq(info.speed, 1);
}
END_TEST

START_TEST(score_of_600_raises_level_to_two) {
  reset_level_game();
  TetrisGame_t *game = tetrisGetGame();
  game->score = 500;
  for (int col = 1; col < TETRIS_COLS; ++col) {
    game->board[TETRIS_ROWS - 1][col] = 1;
  }
  make_vertical_piece(game, 1, TETRIS_ROWS - 1, 0);
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 600);
  ck_assert_int_eq(game->level, 2);
  ck_assert_int_eq(game->speed, 2);
  ck_assert_uint_eq(game->base_fall_delay_ms, 595ULL);
}
END_TEST

START_TEST(score_below_threshold_keeps_level_one) {
  reset_level_game();
  TetrisGame_t *game = tetrisGetGame();
  game->score = 499;
  for (int col = 1; col < TETRIS_COLS; ++col) {
    game->board[TETRIS_ROWS - 1][col] = 1;
  }
  make_vertical_piece(game, 1, TETRIS_ROWS - 1, 0);
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 599);
  ck_assert_int_eq(game->level, 1);
  ck_assert_int_eq(game->speed, 1);
  ck_assert_uint_eq(game->base_fall_delay_ms, 650ULL);
}
END_TEST

START_TEST(level_is_capped_at_ten) {
  reset_level_game();
  TetrisGame_t *game = tetrisGetGame();
  game->score = 5900;
  for (int row = TETRIS_ROWS - 4; row < TETRIS_ROWS; ++row) {
    for (int col = 1; col < TETRIS_COLS; ++col) {
      game->board[row][col] = 1;
    }
  }
  make_vertical_piece(game, 4, TETRIS_ROWS - 4, 0);
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 7400);
  ck_assert_int_eq(game->level, 10);
  ck_assert_int_eq(game->speed, 10);
  ck_assert_uint_eq(game->base_fall_delay_ms, 155ULL);
}
END_TEST

START_TEST(sync_info_exposes_level_and_speed) {
  reset_level_game();
  TetrisGame_t *game = tetrisGetGame();
  game->level = 4;
  game->speed = 4;
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.level, 4);
  ck_assert_int_eq(info.speed, 4);
}
END_TEST

Suite *tetris_level_suite(void) {
  Suite *suite = suite_create("level");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, initial_level_and_speed_are_one);
  tcase_add_test(tcase, score_of_600_raises_level_to_two);
  tcase_add_test(tcase, score_below_threshold_keeps_level_one);
  tcase_add_test(tcase, level_is_capped_at_ten);
  tcase_add_test(tcase, sync_info_exposes_level_and_speed);
  suite_add_tcase(suite, tcase);
  return suite;
}
