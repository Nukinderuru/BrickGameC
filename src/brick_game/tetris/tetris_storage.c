#include "tetris_storage.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define TETRIS_SCORE_PATH_SIZE 4096

static int tetrisBuildPath(char *directory, size_t directory_size, char *file_path,
                           size_t file_size) {
  int ok = 0;
  const char *home = getenv("HOME");
  if (home != NULL && home[0] != '\0') {
    const int dir_len = snprintf(directory, directory_size,
                           "%s/.local/share/brick_game_cli", home);
    const int file_len = snprintf(file_path, file_size,
                            "%s/.local/share/brick_game_cli/high_score", home);
    ok = dir_len > 0 && file_len > 0 && (size_t)dir_len < directory_size &&
         (size_t)file_len < file_size;
  }
  return ok;
}

static void tetrisEnsureDirectory(const char *path) {
  char buffer[TETRIS_SCORE_PATH_SIZE] = {0};
  const size_t len = strlen(path);
  if (len >= sizeof(buffer)) {
    return;
  }
  memcpy(buffer, path, len + 1);
  for (char *cursor = buffer + 1; *cursor != '\0'; ++cursor) {
    if (*cursor == '/') {
      *cursor = '\0';
      if (mkdir(buffer, 0700) != 0 && errno != EEXIST) {
        return;
      }
      *cursor = '/';
    }
  }
  if (mkdir(buffer, 0700) != 0 && errno != EEXIST) {
    return;
  }
}

int tetrisLoadHighScore(void) {
  int high_score = 0;
  char directory[TETRIS_SCORE_PATH_SIZE] = {0};
  char file_path[TETRIS_SCORE_PATH_SIZE] = {0};
  if (!tetrisBuildPath(directory, sizeof(directory), file_path, sizeof(file_path))) {
    return high_score;
  }

  FILE *file = fopen(file_path, "r");
  if (file == NULL) {
    return high_score;
  }

  int loaded = 0;
  if (fscanf(file, "%d", &loaded) == 1 && loaded >= 0) {
    high_score = loaded;
  }
  fclose(file);
  return high_score;
}

void tetrisSaveHighScore(const int high_score) {
  char directory[TETRIS_SCORE_PATH_SIZE] = {0};
  char file_path[TETRIS_SCORE_PATH_SIZE] = {0};
  if (!tetrisBuildPath(directory, sizeof(directory), file_path, sizeof(file_path))) {
    return;
  }

  tetrisEnsureDirectory(directory);
  FILE *file = fopen(file_path, "w");
  if (file == NULL) {
    return;
  }
  fprintf(file, "%d\n", high_score);
  fclose(file);
}
