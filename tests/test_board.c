#include <check.h>

#include <string.h>

#include "../src/brick_game/tetris/tetris_board.h"
#include "../src/brick_game/tetris/tetris_internal.h"

static void reset_game(void) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisReset(game);
  game->state = kStateStart;
  game->paused = false;
  game->game_over = false;
  game->running = false;
  game->exit_requested = false;
}

static Tetromino_t make_single_cell_piece(const int row, const int col) {
  Tetromino_t piece = {0};
  piece.cells[0][0] = 1;
  piece.row = row;
  piece.col = col;
  return piece;
}

START_TEST(can_place_detects_empty_and_occupied_cells) {
  reset_game();
  TetrisGame_t *game = tetrisGetGame();
  const Tetromino_t piece = make_single_cell_piece(2, 2);
  ck_assert_int_eq(tetrisCanPlace(game, &piece), true);
  game->board[2][2] = 1;
  ck_assert_int_eq(tetrisCanPlace(game, &piece), false);
}
END_TEST

START_TEST(merge_current_writes_cells_into_board) {
  reset_game();
  TetrisGame_t *game = tetrisGetGame();
  game->current = make_single_cell_piece(4, 6);
  tetrisMergeCurrent(game);
  ck_assert_int_eq(game->board[4][6], 1);
}
END_TEST

START_TEST(clear_board_zeros_all_cells) {
  reset_game();
  TetrisGame_t *game = tetrisGetGame();
  game->board[0][0] = 1;
  game->board[10][4] = 1;
  tetrisClearBoard(game);
  ck_assert_int_eq(game->board[0][0], 0);
  ck_assert_int_eq(game->board[10][4], 0);
}
END_TEST

START_TEST(rebuild_preview_copies_next_piece_storage) {
  reset_game();
  TetrisGame_t *game = tetrisGetGame();
  game->next_piece = make_single_cell_piece(0, 0);
  tetrisRebuildPreview(game);
  ck_assert_int_eq(game->next_storage[0][0], 1);
  ck_assert_int_eq(game->next_storage[1][0], 0);
}
END_TEST

Suite *tetris_board_suite(void) {
  Suite *suite = suite_create("board");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, can_place_detects_empty_and_occupied_cells);
  tcase_add_test(tcase, merge_current_writes_cells_into_board);
  tcase_add_test(tcase, clear_board_zeros_all_cells);
  tcase_add_test(tcase, rebuild_preview_copies_next_piece_storage);
  suite_add_tcase(suite, tcase);
  return suite;
}
