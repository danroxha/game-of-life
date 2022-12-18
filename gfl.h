#ifndef __GFL_H__
#define __GFL_H__

#define LIVE "█"
#define DEAD " "
#define LINE 1000
#define COL 800

#define MIN_ENABLED_THREAD 1
#define DEFAULT_ENABLED_THREADS 4
#define MAX_ENABLED_NUMBER_THREADS 8

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>
// #include<mpi.h>

struct cell {
  bool alive;
  bool next_generation;
  bool previous_generation;
};

struct game_of_life {
  struct cell board[LINE][COL];
  int collumns;
  int lines;
};

void beacon_shape(struct game_of_life *, int, int );
void blinker_shape(struct game_of_life *, int, int);
void tub_shape(struct game_of_life *, int, int);
void glider_shape(struct game_of_life *, int, int);
void moon_shape(struct game_of_life *, int, int);
void gun_shape(struct game_of_life *, int, int);

void render_board(struct game_of_life, int, int, struct winsize, bool);
struct game_of_life new_game_of_life();
void apply_rules(struct game_of_life *);
void update_generation(struct game_of_life *);
void check_neigborn(struct game_of_life *, int, int);
void generete_seed(struct game_of_life *, int);

struct game_of_life new_game_of_life(int factor) {
  
  struct game_of_life glf = {
    .lines=LINE,
    .collumns=COL
  };

  generete_seed(&glf, factor);
  return glf;
}

void apply_rules(struct game_of_life *gfl) {
  for(int i = 0; i < gfl->lines; i++) { 
    for(int j = 0; j < gfl->collumns; j++) {
      check_neigborn(gfl, i, j);
    }   
  }
}

void check_neigborn(struct game_of_life *gfl, int line, int col) {
  
  int neigborn_alive = 0;

  for(int y = -1; y <= 1; y++) {
    for(int x = -1; x <= 1; x++) {
      if(x == 0 && y == 0) 
        continue;

      int j = col + x;
      int i = line + y;

      if(j >= 0 && j < gfl->collumns && i >= 0 && i < gfl->lines) {
        struct cell neigborn = gfl->board[i][j];
        if(neigborn.alive) {
          neigborn_alive++;
        }  
      }
    }
  }

  if(gfl->board[line][col].alive)
    gfl->board[line][col].next_generation = neigborn_alive >= 2 && neigborn_alive <= 3;
  else
    gfl->board[line][col].next_generation = neigborn_alive == 3;
}

void update_generation(struct game_of_life *gfl) {
  for(int i = 0; i < gfl->lines; i++) {
    for(int j = 0; j < gfl->collumns; j++) {
      gfl->board[i][j].previous_generation = gfl->board[i][j].alive;
      gfl->board[i][j].alive = gfl->board[i][j].next_generation;
    }
  }
}

void render_board(struct game_of_life gfl, int x, int y, struct winsize screen, bool refresh) {
  
  for(int i = 0; i < screen.ws_row; i++) {
    for(int j = 0; j < screen.ws_col; j++) {
      if(gfl.board[i + y][j + x].alive) {
        print(j, i, LIVE);
        continue; 
      }

      if(refresh || gfl.board[i + y][j + x].previous_generation != gfl.board[i + y][j + x].alive) {
        print(j, i, DEAD);
      }
    }
  }
}

void blinker_shape(struct game_of_life *gfl, int x, int y) {
  gfl->board[y][x].alive = true;
  gfl->board[y][x + 1].alive = true;
  gfl->board[y][x + 2].alive = true;
}

void beacon_shape(struct game_of_life *gfl, int x, int y) {
  gfl->board[y][x].alive = true;
  gfl->board[y][x + 1].alive = true;
  gfl->board[y + 1][x].alive = true;
  gfl->board[y + 1][x + 1].alive = true;

  gfl->board[y + 2][x + 2].alive = true;
  gfl->board[y + 2][x + 3].alive = true;
  gfl->board[y + 3][x + 3].alive = true;
  gfl->board[y + 3][x + 2].alive = true;
}

void tub_shape(struct game_of_life *glf, int x, int y) {
  glf->board[ y + 0][x + 1].alive = true;
  glf->board[ y + 1][x + 0].alive = true;
  glf->board[ y + 1][x + 2].alive = true;
  glf->board[ y + 2][x + 1].alive = true;
}

void glider_shape(struct game_of_life *glf, int x, int y) {
  glf->board[y][x].alive = true;
  glf->board[y + 1][x].alive = true;
  glf->board[y + 2][x].alive = true;
  glf->board[y + 2][ x + 1].alive = true;
  glf->board[y + 1][ x + 2].alive = true;
}

void moon_shape(struct game_of_life *glf, int x, int y) {
  glf->board[y + 0][x + 1].alive = true;
  glf->board[y + 1][x + 0].alive = true;
  glf->board[y + 2][x + 0].alive = true;
  glf->board[y + 3][x + 1].alive = true;
}

void gun_shape(struct game_of_life *glf, int x, int y) {

  glf->board[y + 4][x + 0].alive = true;
  glf->board[y + 4][x + 1].alive = true;
  glf->board[y + 5][x + 0].alive = true;
  glf->board[y + 5][x + 1].alive = true;

  glf->board[y + 3][x + 34].alive = true;
  glf->board[y + 3][x + 35].alive = true;
  glf->board[y + 4][x + 34].alive = true;
  glf->board[y + 4][x + 35].alive = true;

  glf->board[y + 2][x + 12].alive = true;
  glf->board[y + 2][x + 13].alive = true;
  glf->board[y + 3][x + 11].alive = true;
  
  glf->board[y + 4][x + 10].alive = true;
  glf->board[y + 5][x + 10].alive = true;
  glf->board[y + 6][x + 10].alive = true;
  glf->board[y + 7][x + 11].alive = true;
  glf->board[y + 8][x + 12].alive = true;
  glf->board[y + 8][x + 13].alive = true;

  glf->board[y + 3][x + 15].alive = true;
  glf->board[y + 5][x + 14].alive = true;
  glf->board[y + 4][x + 16].alive = true;
  glf->board[y + 5][x + 16].alive = true;
  glf->board[y + 5][x + 17].alive = true;
  glf->board[y + 6][x + 16].alive = true;
  glf->board[y + 7][x + 15].alive = true;

  glf->board[y][x + 24].alive = true;
  glf->board[y + 1][x + 24].alive = true;
  glf->board[y + 1][x + 22].alive = true;
  glf->board[y + 2][x + 20].alive = true;
  glf->board[y + 2][x + 21].alive = true;
  glf->board[y + 3][x + 21].alive = true;
  glf->board[y + 3][x + 20].alive = true;
  glf->board[y + 4][x + 20].alive = true;
  glf->board[y + 4][x + 21].alive = true;
  glf->board[y + 5][x + 22].alive = true;
  glf->board[y + 5][x + 24].alive = true;
  glf->board[y + 6][x + 24].alive = true;
}

void generete_seed(struct game_of_life *glf, int factor) {

  srand(time(0));
  for(int i = 0; i <  glf->lines; i++) {
    for(int j = 0; j < glf->collumns; j++) {
      glf->board[i][j].alive = rand() % factor == 1;
      glf->board[i][j].previous_generation = glf->board[i][j].alive;
      glf->board[i][j].next_generation = glf->board[i][j].alive;
    }
  }
}


#endif // __SCREEN_H_