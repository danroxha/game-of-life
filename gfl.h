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
#include<stddef.h>
#include<mpi.h>

#define ROOT_PROCESS 0

struct cell {
  bool alive;
  bool next_generation;
  bool previous_generation;
};

struct game_of_life {
  struct cell board[LINE][COL];
  int collumns;
  int lines;
  int start_i, start_j, end_i, end_j;
} game_of_life;


void beacon_shape(struct game_of_life *, int, int );
void blinker_shape(struct game_of_life *, int, int);
void tub_shape(struct game_of_life *, int, int);
void glider_shape(struct game_of_life *, int, int);
void moon_shape(struct game_of_life *, int, int);
void gun_shape(struct game_of_life *, int, int);

void render_board(struct game_of_life, int, int, struct winsize, bool);
struct game_of_life new_game_of_life();
void apply_rules(struct game_of_life *, MPI_Datatype);
void update_generation(struct game_of_life *);
void check_neigborn(struct game_of_life *, int, int);
void generete_seed(struct game_of_life *, int);
void struct_definition_game_of_life_mpi(MPI_Datatype *);
void fork_jobs_mpi(struct game_of_life *gfl, int, int);
void join_jobs_mpi(struct game_of_life *, struct game_of_life *);


struct game_of_life new_game_of_life(int factor) {
  
  struct game_of_life glf = {
    .lines=LINE,
    .collumns=COL
  };

  generete_seed(&glf, factor);
  return glf;
}

void apply_rules(struct game_of_life *gfl, MPI_Datatype mpi_gfl_type) {
  
  int rank, amount_of_process;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &amount_of_process);
  
  MPI_Status st;

  if(rank == ROOT_PROCESS) {
    for(int nproc = 1; nproc < amount_of_process; nproc++) {
      fork_jobs_mpi(gfl, nproc, amount_of_process);
      MPI_Send(gfl, 1, mpi_gfl_type, nproc , 0, MPI_COMM_WORLD);
      // printf("-> parent - %p\n", gfl);
    }

    for(int nproc = 1; nproc < amount_of_process; nproc++) {
      struct game_of_life s_gfl;
      MPI_Recv(&s_gfl, 1, mpi_gfl_type, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);      
      join_jobs_mpi(gfl, &s_gfl);
    }
  }
  else {
    struct game_of_life s_gfl;
    MPI_Recv(&s_gfl, 1, mpi_gfl_type, 0, 0, MPI_COMM_WORLD, &st);
    // printf("-> child - %p\n", &s_gfl);
    for(int i = s_gfl.start_i; i < s_gfl.end_i; i++) {
      for(int j = s_gfl.start_j; j < s_gfl.end_j; j++) {
        check_neigborn(&s_gfl, i, j);
      }
    }
    MPI_Send(&s_gfl , 1, mpi_gfl_type, ROOT_PROCESS, 1, MPI_COMM_WORLD);
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

void struct_definition_game_of_life_mpi(MPI_Datatype *mpi_gfl_type) {
  const int nitems = 7;
  int blocklengths[7] = {LINE * COL, 1, 1, 1, 1, 1, 1};
  MPI_Datatype types[7] = { MPI_PACKED, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT };
  MPI_Aint offsets[7];
  offsets[0] = offsetof(struct game_of_life, board);
  offsets[1] = offsetof(struct game_of_life, collumns);
  offsets[2] = offsetof(struct game_of_life, lines);
  offsets[3] = offsetof(struct game_of_life, start_i);
  offsets[4] = offsetof(struct game_of_life, start_j);
  offsets[5] = offsetof(struct game_of_life, end_i);
  offsets[6] = offsetof(struct game_of_life, end_j);

  MPI_Type_create_struct(nitems, blocklengths, offsets, types, mpi_gfl_type);
  MPI_Type_commit(mpi_gfl_type);
}

void fork_jobs_mpi(struct game_of_life *gfl, int nproc, int amount_of_process) {

  int row = gfl->lines / amount_of_process;
  int col = gfl->collumns / amount_of_process;

  gfl->start_i = row * (nproc - 1);
  gfl->start_j = col * (nproc - 1);
  gfl->end_i = gfl->start_i + row;
  gfl->end_j = gfl->start_j + col;
}

void join_jobs_mpi(struct game_of_life *gfl, struct game_of_life *s_gfl) {
  for(int i = s_gfl->start_i; i < s_gfl->end_i; i++) {
    for(int j = s_gfl->start_j; j < s_gfl->end_j; j++) {
      gfl->board[i][j].alive = s_gfl->board[i][j].alive;
      gfl->board[i][j].next_generation = s_gfl->board[i][j].next_generation;
      gfl->board[i][j].previous_generation = s_gfl->board[i][j].previous_generation;
    }
  }
}

#endif // __SCREEN_H_