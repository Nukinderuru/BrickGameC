#include <check.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../src/brick_game/tetris/tetris_internal.h"
#include "../src/brick_game/tetris/tetris_logic.h"
#include "../src/brick_game/tetris/tetris_storage.h"

#define TEST_PATH_SIZE 4096

static void make_single_cell_piece(TetrisGame_t *game) {
  memset(game->current.cells, 0, sizeof(game->current.cells));
  game->current.cells[0][0] = 1;
  game->current.type = 0;
  game->current.row = 0;
  game->current.col = 0;
  game->has_current = true;
}

static void build_score_file_path(char *path, size_t size, const char *home) {
  ck_assert_int_lt(
      snprintf(path, size, "%s/.local/share/brick_game_cli/high_score", home),
      (int)size);
}

static void build_path(char *path, size_t size, const char *home,
                       const char *suffix) {
  ck_assert_int_lt(snprintf(path, size, "%s%s", home, suffix), (int)size);
}

static void cleanup_temp_home(const char *home) {
  char path[TEST_PATH_SIZE] = {0};
  build_score_file_path(path, sizeof(path), home);
  unlink(path);
  snprintf(path, sizeof(path), "%s/.local/share/brick_game_cli", home);
  rmdir(path);
  snprintf(path, sizeof(path), "%s/.local/share", home);
  rmdir(path);
  snprintf(path, sizeof(path), "%s/.local", home);
  rmdir(path);
  rmdir(home);
}

static void setup_temp_home(char *home_template, size_t size) {
  snprintf(home_template, size, "/tmp/brick_game_score_XXXXXX");
  ck_assert_ptr_nonnull(mkdtemp(home_template));
  ck_assert_int_eq(setenv("HOME", home_template, 1), 0);
}

START_TEST(lock_without_lines_keeps_score_zero) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->score = 0;
  game->high_score = 999999;
  make_single_cell_piece(game);
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 0);
}
END_TEST

START_TEST(lock_one_line_awards_100_points) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->high_score = 999999;
  for (int col = 1; col < TETRIS_COLS; ++col) {
    game->board[TETRIS_ROWS - 1][col] = 1;
  }
  make_single_cell_piece(game);
  game->current.row = TETRIS_ROWS - 1;
  game->current.col = 0;
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 100);
}
END_TEST

START_TEST(lock_two_lines_awards_300_points) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->high_score = 999999;
  for (int col = 0; col < TETRIS_COLS; ++col) {
    game->board[TETRIS_ROWS - 1][col] = 1;
    game->board[TETRIS_ROWS - 2][col] = 1;
  }
  game->board[TETRIS_ROWS - 1][0] = 0;
  game->board[TETRIS_ROWS - 2][0] = 0;
  make_single_cell_piece(game);
  game->current.row = TETRIS_ROWS - 2;
  game->current.col = 0;
  game->current.cells[1][0] = 1;
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 300);
}
END_TEST

START_TEST(lock_three_lines_awards_700_points) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->high_score = 999999;
  for (int row = TETRIS_ROWS - 3; row < TETRIS_ROWS; ++row) {
    for (int col = 0; col < TETRIS_COLS; ++col) {
      game->board[row][col] = 1;
    }
    game->board[row][0] = 0;
  }
  memset(game->current.cells, 0, sizeof(game->current.cells));
  game->current.cells[0][0] = 1;
  game->current.cells[1][0] = 1;
  game->current.cells[2][0] = 1;
  game->current.row = TETRIS_ROWS - 3;
  game->current.col = 0;
  game->current.type = 0;
  game->has_current = true;
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 700);
}
END_TEST

START_TEST(lock_four_lines_awards_1500_points) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->high_score = 999999;
  for (int row = TETRIS_ROWS - 4; row < TETRIS_ROWS; ++row) {
    for (int col = 0; col < TETRIS_COLS; ++col) {
      game->board[row][col] = 1;
    }
    game->board[row][0] = 0;
  }
  memset(game->current.cells, 0, sizeof(game->current.cells));
  game->current.cells[0][0] = 1;
  game->current.cells[1][0] = 1;
  game->current.cells[2][0] = 1;
  game->current.cells[3][0] = 1;
  game->current.row = TETRIS_ROWS - 4;
  game->current.col = 0;
  game->current.type = 0;
  game->has_current = true;
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->score, 1500);
}
END_TEST

START_TEST(high_score_loads_as_zero_when_file_missing) {
  char temp_home[TEST_PATH_SIZE] = {0};
  setup_temp_home(temp_home, sizeof(temp_home));
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  ck_assert_int_eq(tetrisGetGame()->high_score, 0);
  cleanup_temp_home(temp_home);
}
END_TEST

START_TEST(high_score_persists_when_updated) {
  char temp_home[TEST_PATH_SIZE] = {0};
  setup_temp_home(temp_home, sizeof(temp_home));
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->score = 1200;
  game->high_score = 1000;
  for (int col = 1; col < TETRIS_COLS; ++col) {
    game->board[TETRIS_ROWS - 1][col] = 1;
  }
  make_single_cell_piece(game);
  game->current.row = TETRIS_ROWS - 1;
  game->current.col = 0;
  tetrisLockCurrent(game);
  ck_assert_int_eq(game->high_score, 1300);

  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  ck_assert_int_eq(tetrisGetGame()->high_score, 1300);
  cleanup_temp_home(temp_home);
}
END_TEST

START_TEST(malformed_high_score_file_falls_back_to_zero) {
  char temp_home[TEST_PATH_SIZE] = {0};
  char path[TEST_PATH_SIZE] = {0};
  setup_temp_home(temp_home, sizeof(temp_home));
  build_path(path, sizeof(path), temp_home, "/.local");
  mkdir(path, 0700);
  build_path(path, sizeof(path), temp_home, "/.local/share");
  mkdir(path, 0700);
  build_path(path, sizeof(path), temp_home, "/.local/share/brick_game_cli");
  mkdir(path, 0700);
  build_score_file_path(path, sizeof(path), temp_home);
  FILE *file = fopen(path, "w");
  ck_assert_ptr_nonnull(file);
  fprintf(file, "broken-value\n");
  fclose(file);

  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  ck_assert_int_eq(tetrisGetGame()->high_score, 0);
  cleanup_temp_home(temp_home);
}
END_TEST

START_TEST(negative_high_score_file_falls_back_to_zero) {
  char temp_home[TEST_PATH_SIZE] = {0};
  char path[TEST_PATH_SIZE] = {0};
  setup_temp_home(temp_home, sizeof(temp_home));
  build_path(path, sizeof(path), temp_home, "/.local");
  mkdir(path, 0700);
  build_path(path, sizeof(path), temp_home, "/.local/share");
  mkdir(path, 0700);
  build_path(path, sizeof(path), temp_home, "/.local/share/brick_game_cli");
  mkdir(path, 0700);
  build_score_file_path(path, sizeof(path), temp_home);
  FILE *file = fopen(path, "w");
  ck_assert_ptr_nonnull(file);
  fprintf(file, "-42\n");
  fclose(file);

  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  ck_assert_int_eq(tetrisGetGame()->high_score, 0);
  cleanup_temp_home(temp_home);
}
END_TEST

START_TEST(load_high_score_without_home_returns_zero) {
  ck_assert_int_eq(unsetenv("HOME"), 0);
  ck_assert_int_eq(tetrisLoadHighScore(), 0);
}
END_TEST

START_TEST(load_high_score_with_empty_home_returns_zero) {
  ck_assert_int_eq(setenv("HOME", "", 1), 0);
  ck_assert_int_eq(tetrisLoadHighScore(), 0);
}
END_TEST

START_TEST(save_high_score_without_home_does_not_fail) {
  ck_assert_int_eq(unsetenv("HOME"), 0);
  tetrisSaveHighScore(1234);
  ck_assert_int_eq(tetrisLoadHighScore(), 0);
}
END_TEST

START_TEST(sync_info_exposes_score_and_high_score) {
  tetrisResetSingletonForTests();
  tetrisEnsureInitialized();
  TetrisGame_t *game = tetrisGetGame();
  game->score = 700;
  game->high_score = 1500;
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.score, 700);
  ck_assert_int_eq(info.high_score, 1500);
}
END_TEST

Suite *tetris_score_suite(void) {
  Suite *suite = suite_create("score");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, lock_without_lines_keeps_score_zero);
  tcase_add_test(tcase, lock_one_line_awards_100_points);
  tcase_add_test(tcase, lock_two_lines_awards_300_points);
  tcase_add_test(tcase, lock_three_lines_awards_700_points);
  tcase_add_test(tcase, lock_four_lines_awards_1500_points);
  tcase_add_test(tcase, high_score_loads_as_zero_when_file_missing);
  tcase_add_test(tcase, high_score_persists_when_updated);
  tcase_add_test(tcase, malformed_high_score_file_falls_back_to_zero);
  tcase_add_test(tcase, negative_high_score_file_falls_back_to_zero);
  tcase_add_test(tcase, load_high_score_without_home_returns_zero);
  tcase_add_test(tcase, load_high_score_with_empty_home_returns_zero);
  tcase_add_test(tcase, save_high_score_without_home_does_not_fail);
  tcase_add_test(tcase, sync_info_exposes_score_and_high_score);
  suite_add_tcase(suite, tcase);
  return suite;
}
