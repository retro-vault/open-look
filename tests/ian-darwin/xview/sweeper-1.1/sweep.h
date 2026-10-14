/*static char sccsid[]="@(#)sweep.h	1.1 9/30/92";*/
#include <sys/types.h>
#include <stdio.h>

#define BOX_SIZE 30
#define OFFSET 7

#define S10X10 0
#define S15X15 1
#define S18X25 2

#define BOMB -1
#define NOHIGHSCORES 10

typedef enum markval {UNMARKED,CHECK,SHOWN,FLAG} marktype;

#ifdef _IN_SWEEP_C_
#define SWEEP_STORAGE
#else
#define SWEEP_STORAGE extern
#endif

SWEEP_STORAGE int area_width,area_height;
SWEEP_STORAGE int game_started;
SWEEP_STORAGE int no_bombs,no_marked_bombs,no_cor_marked_bombs;
SWEEP_STORAGE int level;
SWEEP_STORAGE int size;
SWEEP_STORAGE long seed;

typedef struct stackstruct {
      int x;
      int y;
      struct stackstruct *next;
      struct stackstruct *prev;
} stacktype;

typedef struct movestruct {
      int x;
      int y;
      int flag;
} movetype;

SWEEP_STORAGE stacktype *stack,*top;

struct bombs {
      int value;
      marktype  selected;
};
SWEEP_STORAGE struct bombs bomb_grid[18][25];

typedef struct highscoreentry {
      char name[30];
      int score;
} highscoretype; 

SWEEP_STORAGE highscoretype highscore[5][3][NOHIGHSCORES];

void init_board();
void push();
void pop();
void flag();
movetype *input_move();
void show_board();
#ifdef _IN_SWEEP_C_

 double drand48();
 void srand48();

#endif
