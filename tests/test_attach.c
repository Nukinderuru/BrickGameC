#include <check.h>

#include <string.h>

#include "../src/brick_game/tetris/tetris_board.h"
#include "../src/brick_game/tetris/tetris_internal.h"

static void set_single_cell_piece(TetrisGame_t *game, const int row, const int col) {
  memset(game->current.cells, 0, sizeof(game->current.cells));
  game->current.cells[0][0] = 1;
  game->current.type = 0;
  game->current.row = row;
  game->current.col = col;
  game->has_current = true;
}

static void setup_attach_game(void) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisReset(game);
  game->state = kStateStart;
  userInput(Start, false);
  updateCurrentState();
}

START_TEST(piece_attaches_at_floor) {
  setup_attach_game();
  TetrisGame_t *game = tetrisGetGame();
  game->current.row = TETRIS_ROWS - 2;
  game->last_tick_ms = 0;
  updateCurrentState();
  ck_assert_int_ne(game->state, kStateMoving);
}
END_TEST

START_TEST(full_row_is_cleared) {
  setup_attach_game();
  TetrisGame_t *game = tetrisGetGame();
  for (int col = 0; col < TETRIS_COLS; ++col) {
    game->board[TETRIS_ROWS - 1][col] = 1;
  }
  tetrisClearLines(game);
  for (int col = 0; col < TETRIS_COLS; ++col) {
    ck_assert_int_eq(game->board[TETRIS_ROWS - 1][col], 0);
  }
}
END_TEST

START_TEST(attaching_in_top_row_enters_game_over) {
  setup_attach_game();
  TetrisGame_t *game = tetrisGetGame();
  set_single_cell_piece(game, 0, 0);
  game->state = kStateAttaching;
  updateCurrentState();
  ck_assert_int_eq(game->state, kStateGameOver);
  ck_assert_int_eq(game->game_over, true);
}
END_TEST

START_TEST(spawn_uses_previous_preview_piece) {
  setup_attach_game();
  TetrisGame_t *game = tetrisGetGame();
  Tetromino_t preview = game->next_piece;
  game->state = kStateSpawn;
  game->has_current = false;
  updateCurrentState();
  for (int row = 0; row < TETROMINO_SIZE; ++row) {
    for (int col = 0; col < TETROMINO_SIZE; ++col) {
      ck_assert_int_eq(game->current.cells[row][col], preview.cells[row][col]);
    }
  }
}
END_TEST

START_TEST(sync_info_renders_active_piece_over_board) {
  setup_attach_game();
  TetrisGame_t *game = tetrisGetGame();
  set_single_cell_piece(game, 5, 4);
  game->board[5][4] = 0;
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.field[5][4], game->current.type + 1);
}
END_TEST

Suite *tetris_attach_suite(void) {
  Suite *suite = suite_create("attach");
  TCase *tcase = tcase_create("core");
  tcase_add_test(tcase, piece_attaches_at_floor);
  tcase_add_test(tcase, full_row_is_cleared);
  tcase_add_test(tcase, attaching_in_top_row_enters_game_over);
  tcase_add_test(tcase, spawn_uses_previous_preview_piece);
  tcase_add_test(tcase, sync_info_renders_active_piece_over_board);
  suite_add_tcase(suite, tcase);
  return suite;
}
