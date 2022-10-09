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
#include<omp.h>

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

struct array_range {
  int start;
  int end;
};

void beacon_shape(struct game_of_life *, int, int );
void blinker_shape(struct game_of_life *, int, int);
void tub_shape(struct game_of_life *, int, int);
void glider_shape(struct game_of_life *, int, int);
void moon_shape(struct game_of_life *, int, int);
void gun_shape(struct game_of_life *, int, int);

void render_board(struct game_of_life, int, int, struct winsize, bool);
struct game_of_life new_game_of_life();
struct array_range thread_workspace_range(int, int, int) ;
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

  // gun_shape(&glf, 10, 10);
  // gun_shape(&glf, 110, 10);
  // gun_shape(&glf, 170, 10);
  // gun_shape(&glf, 220, 10);
  // gun_shape(&glf, 280, 10);
  // gun_shape(&glf, 340, 10);
  // gun_shape(&glf, 390, 10);
  // gun_shape(&glf, 450, 10);
  // gun_shape(&glf, 500, 10);
  // gun_shape(&glf, 550, 10);
  // gun_shape(&glf, 600, 10);
  // gun_shape(&glf, 650, 10);
  // gun_shape(&glf, 750, 10);
  // gun_shape(&glf, 10, 50);
  // gun_shape(&glf, 10, 100);
  // gun_shape(&glf, 0, 121);

  return glf;
}

void apply_rules(struct game_of_life *gfl) {
  #pragma omp parallel num_threads(MAX_ENABLED_NUMBER_THREADS) 
  {
    struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), gfl->lines);
    /*
      [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ...799]
      t = 8; lines = 800;
      id = 0; 
        range_working_thread = () 1000 / 8 = 125
        initial_i = 0 * 125 = 0
        end_i = 0 + 125 = 125

      id = 1;
        range_working_thread = () 1000 / 8 = 125
        initial_i = 1 * 125 = 125
        end_i = 125 + 125 = 250

      id = 2;
        range_working_thread = () 1000 / 8 = 125
        initial_i = 2 * 125 = 250
        end_i = 250 + 125 = 375

      id = ;
        range_working_thread = () 1000 / 8 = 125
        initial_i = 2 * 125 = 250
        end_i = 250 + 125 = 375
      
    * /

    /**
     * [
     *   [0x1, 0x2, 0x3], -> thread 1 (i: thread_id(1) * range_working_thread )
     *   [0x4, 0x5, 0x6], -> thread 2 (i: thread_id(2) * range_working_thread )
     *   [0x7, 0x8, 0x0], -> thread 3 (i: thread_id(3) * range_working_thread )
     * ]
     * */

    for(int i = range.start; i < range.end; i++) {
      #pragma omp parallel num_threads(MAX_ENABLED_NUMBER_THREADS)
      {
        /**
         *  [ 0x1,      0x2,      0x3  ]
         *     |         |         |
         *  thread 4  thread 5  thread 6
         * 
         * */
        struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), gfl->collumns);
        
        for(int j = range.start; j < range.end; j++) {
          check_neigborn(gfl, i, j);
        }
      }
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
  #pragma omp parallel num_threads(MAX_ENABLED_NUMBER_THREADS) 
  {
    struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), gfl->lines);

    for(int i = range.start; i < range.end; i++) {     
      #pragma omp parallel num_threads(MAX_ENABLED_NUMBER_THREADS)
      {
        struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), gfl->collumns);

        for(int j = range.start; j < range.end; j++) {
          gfl->board[i][j].previous_generation = gfl->board[i][j].alive;
          gfl->board[i][j].alive = gfl->board[i][j].next_generation;
        }
      }
    }
  }
}

void render_board(struct game_of_life gfl, int x, int y, struct winsize screen, bool refresh) {
  
  omp_lock_t output_lock;
  omp_init_lock(&output_lock);
  
  
  #pragma omp parallel num_threads(DEFAULT_ENABLED_THREADS) 
  {
    struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), screen.ws_row);

    for(int i = range.start; i < range.end; i++) {
      if(i > gfl.lines)
        break;

      #pragma omp parallel num_threads(DEFAULT_ENABLED_THREADS) 
      {
        struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), screen.ws_col);

        for(int j = range.start; j < range.end; j++) {

          if(j > gfl.collumns)
            break;

          if(gfl.board[i + y][j + x].alive) {
            omp_set_lock(&output_lock);
            print(j, i, LIVE);
            omp_unset_lock(&output_lock);
            continue; 
          }

          if(refresh || gfl.board[i + y][j + x].previous_generation != gfl.board[i + y][j + x].alive) {
            omp_set_lock(&output_lock);
            print(j, i, DEAD);
            omp_unset_lock(&output_lock);
          }
        }
      }
    }
  }

  omp_destroy_lock(&output_lock);
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
  #pragma omp parallel
  {
    struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), glf->lines);

    for(int i = range.start; i < range.end; i++) {
      
      #pragma omp parallel num_threads(DEFAULT_ENABLED_THREADS) 
      {
        struct array_range range = thread_workspace_range(omp_get_thread_num(), omp_get_num_threads(), glf->collumns);

        for(int j = range.start; j < range.end; j++) {
          glf->board[i][j].alive = rand() % factor == 1;
          glf->board[i][j].previous_generation = glf->board[i][j].alive;
          glf->board[i][j].next_generation = glf->board[i][j].alive;
        }
      }
    }
  }
}

struct array_range thread_workspace_range(int thread_id, int num_threads, int size) {
  int length = (int) size / num_threads;
  int start = thread_id * length;
  int end = start + length;
  
  struct array_range range = {.start = start, .end=end };
  return range;
}

#endif // __SCREEN_H__