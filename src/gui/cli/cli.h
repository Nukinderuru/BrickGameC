#ifndef SRC_GUI_CLI_CLI_H_
#define SRC_GUI_CLI_CLI_H_

#include <stdbool.h>

#include "../../brick_game/tetris/tetris.h"

void cliProcessInput(int ch);
bool cliShouldExit(void);
void cliRender(const GameInfo_t *info);

#endif  // SRC_GUI_CLI_CLI_H_
