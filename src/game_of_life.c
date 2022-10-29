#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 25
#define COLUMNS 80

void StartScreen();
void EndScreen(int number_of_generation);

void InitializeZeroGeneration(int generation[][COLUMNS]);
void CopyGeneration(int current_generation[][COLUMNS],
                    int previous_generation[][COLUMNS]);
int CompareGenerations(int current_generation[][COLUMNS],
                       int previous_generation[][COLUMNS]);

int CountLiveNeighbours(int generation[][COLUMNS], int x, int y);
int MakeNewGeneration(int current_generation[][COLUMNS],
                      int previous_generation[][COLUMNS]);

void PrintGeneration(int generation[][COLUMNS], int number_of_generation);

void CommandProcessing(char command, int *game_speed, int *end_of_game);

int main() {
  int current_generation[ROWS][COLUMNS] = {0};
  int previous_generation[ROWS][COLUMNS] = {0};
  int number_of_generation = 0;
  int game_speed = 100000;
  int end_of_game = 0;

  initscr();
  noecho();
  raw();
  nodelay(stdscr, TRUE);
  curs_set(0);

  StartScreen();
  InitializeZeroGeneration(current_generation);

  FILE *tty = freopen("/dev/tty", "r", stdin);
  if (tty == NULL) {
    printw("Open terminal for reading");
  }

  while (end_of_game == 0) {
    number_of_generation++;

    PrintGeneration(current_generation, number_of_generation);
    usleep(game_speed);

    end_of_game = MakeNewGeneration(current_generation, previous_generation);

    char command = getch();
    CommandProcessing(command, &game_speed, &end_of_game);
  }

  EndScreen(number_of_generation);
  endwin();
  return 0;
}

void StartScreen() {
  clear();

  printw("   ____                                         __     _       _    "
         "__        \n");
  printw("  / ___|   __ _   _ __ ___     ___      ___    / _|   | |     (_)  / "
         "_|   ___ \n");
  printw(" | |  _   / _` | | '_ ` _ \\   / _ \\    / _ \\  | |_    | |     | | "
         "| |_   / _ \\\n");
  printw(" | |_| | | (_| | | | | | | | |  __/   | (_) | |  _|   | |___  | | |  "
         "_| |  __/\n");
  printw("  \\____|  \\__,_| |_| |_| |_|  \\___|    \\___/  |_|     |_____| "
         "|_| |_|    \\___|\n\n\n");
  printw("                                       Controls\n\n");
  printw("       ,-'-,                   ,-'-,                   ,-'-,\n");
  printw("       | Q | - exit game       | A | - speed up        | Z | - "
         "speed down\n");
  printw("       '--,'                   '--,'                   '--,'\n");

  refresh();
  usleep(3000000);
}

void EndScreen(int number_of_generation) {
  clear();

  printw(
      "   ____                                 ___                        \n");
  printw(
      "  / ___|   __ _   _ __ ___     ___     / _ \\  __   __   ___   _ __ \n");
  printw(" | |  _   / _` | | '_ ` _ \\   / _ \\   | | | | \\ \\ / /  / _ \\ | "
         "'__|\n");
  printw(
      " | |_| | | (_| | | | | | | | |  __/   | |_| |  \\ V /  |  __/ | |   \n");
  printw("  \\____|  \\__,_| |_| |_| |_|  \\___|    \\___/    \\_/    \\___| "
         "|_|   \n\n");
  printw("                       You lived %d generations",
         number_of_generation);

  refresh();
  usleep(3000000);
}

void InitializeZeroGeneration(int generation[][COLUMNS]) {
  int x, y;

  PrintGeneration(generation, 0);

  while ((scanf("%d %d", &x, &y) == 2) && (x >= 0) && (x < ROWS) && (y >= 0) &&
         (y < COLUMNS)) {
    generation[x][y] = 1;
    PrintGeneration(generation, 0);
  }
}

void CopyGeneration(int current_generation[][COLUMNS],
                    int previous_generation[][COLUMNS]) {
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLUMNS; ++j) {
      previous_generation[i][j] = current_generation[i][j];
    }
  }
}

int CompareGenerations(int current_generation[][COLUMNS],
                       int previous_generation[][COLUMNS]) {
  int is_equal = 1;
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLUMNS; ++j) {
      if (current_generation[i][j] != previous_generation[i][j]) {
        is_equal = 0;
      }
    }
  }
  return is_equal;
}

int CountLiveNeighbours(int generation[][COLUMNS], int x, int y) {
  int sum = 0;

  sum += generation[(x + ROWS - 1) % ROWS][(y + COLUMNS - 1) % COLUMNS];
  sum += generation[(x + ROWS - 1) % ROWS][(y + COLUMNS) % COLUMNS];
  sum += generation[(x + ROWS - 1) % ROWS][(y + COLUMNS + 1) % COLUMNS];
  sum += generation[(x + ROWS) % ROWS][(y + COLUMNS - 1) % COLUMNS];
  sum += generation[(x + ROWS) % ROWS][(y + COLUMNS + 1) % COLUMNS];
  sum += generation[(x + ROWS + 1) % ROWS][(y + COLUMNS - 1) % COLUMNS];
  sum += generation[(x + ROWS + 1) % ROWS][(y + COLUMNS) % COLUMNS];
  sum += generation[(x + ROWS + 1) % ROWS][(y + COLUMNS + 1) % COLUMNS];

  return sum;
}

int MakeNewGeneration(int current_generation[][COLUMNS],
                      int previous_generation[][COLUMNS]) {
  int deads_counter = 0;

  CopyGeneration(current_generation, previous_generation);

  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLUMNS; ++j) {
      int prev_gen_ceil = previous_generation[i][j];
      int neigbours_counter = CountLiveNeighbours(previous_generation, i, j);

      if (prev_gen_ceil == 0) {
        if (neigbours_counter == 3) {
          current_generation[i][j] = 1;
        } else {
          current_generation[i][j] = 0;
        }
      } else {
        if ((neigbours_counter == 2) || (neigbours_counter == 3)) {
          current_generation[i][j] = 1;
        } else {
          current_generation[i][j] = 0;
        }
      }

      if (current_generation[i][j] == 0) {
        deads_counter++;
      }
    }
  }

  if (CompareGenerations(current_generation, previous_generation) == 1) {
    deads_counter = ROWS * COLUMNS;
  }

  return deads_counter == (ROWS * COLUMNS);
}

void PrintGeneration(int generation[][COLUMNS], int number_of_generation) {
  clear();

  printw("Generation #%d\n", number_of_generation);
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLUMNS; ++j) {
      if (generation[i][j] == 0) {
        printw(".");
      } else {
        printw("*");
      }
    }
    printw("\n");
  }

  refresh();
}

void CommandProcessing(char command, int *game_speed, int *end_of_game) {
  if (command == 'A' || command == 'a') {
    if (*game_speed > 0) {
      *game_speed -= 10000;
    }
  } else if (command == 'Z' || command == 'z') {
    if (*game_speed < 220000) {
      *game_speed += 10000;
    }
  } else if (command == 'Q' || command == 'q') {
    *end_of_game = 1;
  }
}
