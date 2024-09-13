#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define ANSI_CLEAR "\033[2J"
#define ANSI_MOVETO_ORIGIN "\033[H"

#define ANSI_DEFAULT "\033[0m"
#define ANSI_RED "\033[1;31m"

#define WIDTH 4

void ANSI_256COLOR(int color) { printf("\033[38;5;%dm", color); }

int board[WIDTH][WIDTH] = {0};
int empty[WIDTH * WIDTH][2];
int emptyCount;

void init() {
  srand(time(NULL));
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int score = 0, isDead = 0;

void update() {
  char ch = getchar();
  if (ch != 'w' && ch != 'a' && ch != 's' && ch != 'd') {
    return;
  }

  if (ch == 'w') {
    for (int j = 0; j < WIDTH; j++) {
      for (int i = 0; i < WIDTH - 1; i++) {
        if (board[i][j] != 0) {
          int next = i + 1;
          while (next < WIDTH && board[next][j] == 0) {
            next++;
          }
          if (next < WIDTH && board[i][j] == board[next][j]) {
            board[i][j] *= 2;
            board[next][j] = 0;
            score += board[i][j];
          }
        }
      }
      int k = 0;
      for (int i = 0; i < WIDTH; i++) {
        if (board[i][j] != 0) {
          if (k != i) {
            board[k][j] = board[i][j];
            board[i][j] = 0;
          }
          k++;
        }
      }
    }
  }
  if (ch == 'a') {
    for (int i = 0; i < WIDTH; i++) {
      for (int j = 0; j < WIDTH - 1; j++) {
        if (board[i][j] != 0) {
          int next = j + 1;
          while (next < WIDTH && board[i][next] == 0) {
            next++;
          }
          if (next < WIDTH && board[i][j] == board[i][next]) {
            board[i][j] *= 2;
            board[i][next] = 0;
            score += board[i][j];
          }
        }
      }
      int k = 0;
      for (int j = 0; j < WIDTH; j++) {
        if (board[i][j] != 0) {
          if (k != j) {
            board[i][k] = board[i][j];
            board[i][j] = 0;
          }
          k++;
        }
      }
    }
  }
  if (ch == 's') {
    for (int j = 0; j < WIDTH; j++) {
      for (int i = WIDTH - 1; i > 0; i--) {
        if (board[i][j] != 0) {
          int next = i - 1;
          while (next >= 0 && board[next][j] == 0) {
            next--;
          }
          if (next >= 0 && board[i][j] == board[next][j]) {
            board[i][j] *= 2;
            board[next][j] = 0;
            score += board[i][j];
          }
        }
      }
      int k = WIDTH - 1;
      for (int i = WIDTH - 1; i >= 0; i--) {
        if (board[i][j] != 0) {
          if (k != i) {
            board[k][j] = board[i][j];
            board[i][j] = 0;
          }
          k--;
        }
      }
    }
  }
  if (ch == 'd') {
    for (int i = 0; i < WIDTH; i++) {
      for (int j = WIDTH - 1; j > 0; j--) {
        if (board[i][j] != 0) {
          int next = j - 1;
          while (next >= 0 && board[i][next] == 0) {
            next--;
          }
          if (next >= 0 && board[i][j] == board[i][next]) {
            board[i][j] *= 2;
            board[i][next] = 0;
            score += board[i][j];
          }
        }
      }
      int k = WIDTH - 1;
      for (int j = WIDTH - 1; j >= 0; j--) {
        if (board[i][j] != 0) {
          if (k != j) {
            board[i][k] = board[i][j];
            board[i][j] = 0;
          }
          k--;
        }
      }
    }
  }

  emptyCount = 0;

  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (board[i][j] == 0) {
        empty[emptyCount][0] = i;
        empty[emptyCount][1] = j;
        emptyCount++;
      }
    }
  }
  if (emptyCount) {
    int target = rand() % emptyCount;
    int value = rand() % 2 ? 2 : 4;
    board[empty[target][0]][empty[target][1]] = value;
  } else {
    for (int i = 0; i < WIDTH; i++) {
      for (int j = 0; j < WIDTH - 1; j++) {
        if (board[i][j] == board[i][j + 1]) {
          return;
        }
      }
    }
    for (int j = 0; j < WIDTH; j++) {
      for (int i = 0; i < WIDTH - 1; i++) {
        if (board[i][j] == board[i + 1][j]) {
          return;
        }
      }
    }
    isDead = 1;
  }
}

void render() {
  puts(ANSI_CLEAR);
  puts(ANSI_MOVETO_ORIGIN);
  for (int i = 0; i < WIDTH; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if (board[i][j] == 0) {
        printf("\t");
      } else {
        int level = log2(board[i][j]);
        ANSI_256COLOR(16 + level);
        printf("%d\t", board[i][j]);
      }
    }
    puts(ANSI_DEFAULT);
  }
  printf("Score: %d\n", score);
}

int main() {
  init();
  puts(ANSI_CLEAR);
  puts(ANSI_MOVETO_ORIGIN);
  puts("Welcome to 2048!");
  puts("Use WASD to move the tiles.");
  puts("Move to start!");

  while (!isDead) {
    update();
    render();
  }

  printf("%sGame Over!%s", ANSI_RED, ANSI_DEFAULT);

  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= ICANON | ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
  return 0;
}
