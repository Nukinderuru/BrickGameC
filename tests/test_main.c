#include <check.h>

Suite *tetris_state_suite(void);
Suite *tetris_move_suite(void);
Suite *tetris_rotation_suite(void);
Suite *tetris_attach_suite(void);
Suite *tetris_board_suite(void);

int main(void) {
  int failed = 0;
  SRunner *runner = srunner_create(tetris_state_suite());
  srunner_add_suite(runner, tetris_move_suite());
  srunner_add_suite(runner, tetris_rotation_suite());
  srunner_add_suite(runner, tetris_attach_suite());
  srunner_add_suite(runner, tetris_board_suite());
  srunner_run_all(runner, CK_NORMAL);
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return failed == 0 ? 0 : 1;
}
