#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>

#include"screen.h"
#include"keyboard.h"

// #define LINE 30
// #define COL 100

#define LINE 1000
#define COL 800
#define SEED 1

#define LIVE "█"
#define DEAD " "

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

void render_board(struct game_of_life, int, int, struct winsize);
struct game_of_life new_game_of_life();
void apply_rules(struct game_of_life *);
void update_generation(struct game_of_life *);
void check_neigborn(struct game_of_life *, int, int);

void beacon_shape(struct game_of_life *, int, int );
void blinker_shape(struct game_of_life *, int, int);
void tub_shape(struct game_of_life *, int, int);
void glider_shape(struct game_of_life *, int, int);
void moon_shape(struct game_of_life *, int, int);
void gun_shape(struct game_of_life *, int, int);

void generete_seed(struct game_of_life *, int);

int main(int argc, char** argv) {

  nocursor();
  srand(time(0));


  int factor = 0;

  if(argc >= 2) {
    factor = atoi(argv[1]);
  }

  if(factor == 0) {
    factor = 10;
  }

  struct game_of_life gfl = new_game_of_life(factor);

  int key;
  int counter = 0;
  int generation = 0;

  struct winsize ws;
  int tty;

  tty = open("/dev/tty", O_RDWR);
  if(tty < 0 || ioctl(tty, TIOCGWINSZ, &ws) < 0) 
    err(8, "/dev/tty");

  bool loop = true;

  int screen_x = 0;
  int screen_y = 0;

  while(loop) {
    
    apply_rules(&gfl);

    if(tty < 0 || ioctl(tty, TIOCGWINSZ, &ws) < 0) 
      err(8, "/dev/tty");

    
    // while(++counter <= 50000) {
      gotoxy(0, LINE + 1);
      printf("BOARD[%d x %d]:[%d, %d]: Geração: %d -  (%d)", LINE, COL, screen_x, screen_y, generation, counter);
    // }

    if (kbhit()) {
      key = readch();
      switch(key) {
        case KEY_ESC:
        case KEY_ENTER: {
          loop = false;
          break;
        }
        case KEY_R_U:
        case KEY_R_L: {
          gfl = new_game_of_life(factor);
          system("clear");
          generation = 0;
          
          continue;
        }
        case KEY_S_U:
        case KEY_S_L: {
          screen_y+=5;
          if(screen_y >= gfl.lines - ws.ws_row) {
            screen_y = gfl.lines - ws.ws_row;
          }
          break;
        }
        case KEY_W_U:
        case KEY_W_L: {
          screen_y-= 5;
          if(screen_y <= 0)
            screen_y = 0;
          break;
        }
        case KEY_D_U:
        case KEY_D_L: {
          screen_x+=5;
          if(screen_x >= gfl.collumns - ws.ws_col)
            screen_x =  gfl.collumns - ws.ws_col;
          break;
        }
        case KEY_A_U:
        case KEY_A_L: {
          screen_x-=5;
          if(screen_x <= 0)
            screen_x = 0;
          break;
        }
        case KEY_H_L:
        case KEY_H_U: {
          system("clear");
          break;
        }
      }
    }

    render_board(gfl, screen_x, screen_y, ws);
    update_generation(&gfl);
    
    counter = 0;
    generation++;
  }

  showcursor();
  close(tty);

  system("reset");
  
  return 0;
}

struct game_of_life new_game_of_life(int factor) {
  
  struct game_of_life glf = {
    .lines=LINE,
    .collumns=COL
  };

  // generete_seed(&glf, factor);

  tub_shape(&glf, 0, 4);
  beacon_shape(&glf, 10, 4);
  blinker_shape(&glf, 20, 5);
  glider_shape(&glf, 30, 4);
  moon_shape(&glf, 40, 4); 
  gun_shape(&glf, 50, 10);
  gun_shape(&glf, 90, 10);
  gun_shape(&glf, 130, 10);
  gun_shape(&glf, 170, 10);

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
  
  /**
   *  0 0 0
   *  0 1 0
   *  0 0 0
   *
   * */

  for(int y = -1; y <= 1; y++) {
    for(int x = -1; x <= 1; x++) {
      if(x == 0 && y == 0) 
        continue;

      int j = col + x;
      int i = line + y;

      if(j >= 0 && j < COL && i >= 0 && i < LINE) {
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

void render_board(struct game_of_life gfl, int x, int y, struct winsize screen) {
  for(int i = 0; i < screen.ws_row - 1; i++) {

    if(i >= gfl.lines)
      break;

    for(int j = 0; j < screen.ws_col; j++) {

      if(j >= gfl.collumns)
        break;

      if(gfl.board[i + y][j + x].alive) {
        print(j + 1, i + 1, LIVE);
        continue; 
      }

      print(j + 1, i + 1, DEAD);
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
  for(int i = 0; i < glf->lines; i++) {
    for(int j = 0; j < glf->collumns; j++) {
      glf->board[i][j].alive = rand() % factor == 1;
      glf->board[i][j].previous_generation = glf->board[i][j].alive;
      glf->board[i][j].next_generation = glf->board[i][j].alive;
    }
  }
}
