#include "tetris.h"

#include "tetris_fsm.h"

void userInput(const UserAction_t action, const bool hold) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisHandleInput(action, hold);
  tetrisSyncInfo(game);
}

GameInfo_t updateCurrentState(void) {
  TetrisGame_t *game = tetrisGetGame();
  tetrisEnsureInitialized();
  tetrisUpdate(game);
  tetrisSyncInfo(game);
  return game->info;
}
