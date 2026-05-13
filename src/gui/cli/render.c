#include "cli.h"

#include <ncurses.h>
#include <string.h>

#include "../../brick_game/tetris/tetris_internal.h"

static void drawCell(const int row, const int col, const int value) {
  if (value > 0 && has_colors()) {
    attron(COLOR_PAIR(value));
    mvprintw(row, col, "[]");
    attroff(COLOR_PAIR(value));
  } else {
    mvprintw(row, col, value ? "[]" : "  ");
  }
}

static void drawField(const GameInfo_t *info) {
  mvprintw(1, 2, "BrickGame v1.0: Tetris");
  for (int row = 0; row < 20; ++row) {
    mvprintw(row + 3, 2, "|");
    for (int col = 0; col < 10; ++col) {
      drawCell(row + 3, col * 2 + 3, info->field[row][col]);
    }
    mvprintw(row + 3, 23, "|");
  }
  mvprintw(23, 2, "+--------------------+");
}

static void drawSidebar(const GameInfo_t *info) {
  mvprintw(3, 28, "Next:");
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      drawCell(row + 4, col * 2 + 28, info->next[row][col]);
    }
  }
  mvprintw(10, 28, "Score: %d", info->score);
  mvprintw(11, 28, "High:  %d", info->high_score);
  mvprintw(12, 28, "Level: %d", info->level);
  mvprintw(13, 28, "Speed: %d", info->speed);
  mvprintw(14, 28, "Pause: %s", info->pause ? "yes" : "no");
  mvprintw(16, 28, "Enter : start");
  mvprintw(17, 28, "P     : pause");
  mvprintw(18, 28, "Q/Esc : quit");
  mvprintw(19, 28, "Arrows: move");
  mvprintw(20, 28, "Space : rotate");
}

static void drawOverlay(void) {
  const TetrisGame_t *game = tetrisGetGame();
  const char *line1 = NULL;
  const char *line2 = NULL;

  if (game->state == kStateStart) {
    line1 = "PRESS ENTER";
    line2 = "TO START";
  } else if (game->state == kStateGameOver) {
    line1 = "GAME OVER";
    line2 = "PRESS ENTER";
  } else if (game->paused) {
    line1 = "PAUSED";
    line2 = "PRESS P";
  }

  if (line1 != NULL) {
    const int box_top = 9;
    const int box_left = 5;
    const int box_width = 16;
    const int box_height = 6;
    const int inner_width = box_width - 2;
    char border[17] = {0};
    char blank[15] = {0};
    const int line1_col = box_left + 1 + (inner_width - (int)strlen(line1)) / 2;
    const int line2_col = box_left + 1 + (inner_width - (int)strlen(line2)) / 2;

    for (int i = 0; i < box_width; ++i) {
      border[i] = (i == 0 || i == box_width - 1) ? '+' : '-';
    }
    for (int i = 0; i < inner_width; ++i) {
      blank[i] = ' ';
    }

    mvprintw(box_top, box_left, "%s", border);
    for (int row = 1; row < box_height - 1; ++row) {
      mvprintw(box_top + row, box_left, "|");
      mvprintw(box_top + row, box_left + 1, "%s", blank);
      mvprintw(box_top + row, box_left + box_width - 1, "|");
    }
    mvprintw(box_top + box_height - 1, box_left, "%s", border);
    mvprintw(box_top + 2, line1_col, "%s", line1);
    mvprintw(box_top + 3, line2_col, "%s", line2);
  }
}

void cliRender(const GameInfo_t *info) {
  erase();
  drawField(info);
  drawSidebar(info);
  drawOverlay();
  refresh();
}
