#include "cli.h"

#include <locale.h>
#include <ncurses.h>
#include <time.h>

int main(void) {
  setlocale(LC_ALL, "");
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);
  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_CYAN, -1);
    init_pair(2, COLOR_YELLOW, -1);
    init_pair(3, COLOR_MAGENTA, -1);
    init_pair(4, COLOR_GREEN, -1);
    init_pair(5, COLOR_RED, -1);
    init_pair(6, COLOR_BLUE, -1);
    init_pair(7, COLOR_WHITE, -1);
  }

  int running = 1;
  while (running) {
    const int ch = getch();
    if (ch != ERR) {
      cliProcessInput(ch);
    }
    GameInfo_t info = updateCurrentState();
    cliRender(&info);
    if (cliShouldExit()) {
      running = 0;
    }
    struct timespec sleep_time = {.tv_sec = 0, .tv_nsec = 16000000L};
    nanosleep(&sleep_time, NULL);
  }

  endwin();
  return 0;
}
