#include "cli.h"

#include <ncurses.h>

static bool g_should_exit = false;

void cliProcessInput(const int ch) {
  switch (ch) {
    case '\n':
    case KEY_ENTER:
      userInput(Start, false);
      break;
    case 'p':
    case 'P':
      userInput(Pause, false);
      break;
    case 'q':
    case 'Q':
    case 27:
      userInput(Terminate, false);
      g_should_exit = true;
      break;
    case KEY_LEFT:
      userInput(Left, false);
      break;
    case KEY_RIGHT:
      userInput(Right, false);
      break;
    case KEY_DOWN:
      userInput(Down, false);
      break;
    case ' ':
      userInput(Action, false);
      break;
    default:
      break;
  }
}

bool cliShouldExit(void) { return g_should_exit; }
