#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h> 


// Jason
// Global variables for game features
// MAX_SNAKE set to 300 to support half-perimeter win condition
 
#define MAX_SNAKE 300
int pending_growth = 0; 

// Laiba. directions for the snake basic movement.
// moving up down left and right.
//using enum instead of words for comparing code, easier
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// Laiba Sajjad. each part of the snake is x y, its a point on the screen.
typedef struct {
    int x;
    int y;
} Point;

// Laiba. this is the full snake info body- all snake parts
// length- how big/long the snake is currenty
//dir- shows which movement or way snake goes in. left right, etc.
typedef struct {
    Point body[MAX_SNAKE];
    int length;
    Direction dir;
} Snake;

// Laiba. void init_snake: vvvvv
//creates the snake at the start
//startX and startY- WHERE the snake starts on the screen
// Initial length- set as 5. (starting size of our snake)
// the snake will naturally go right as default.
//building snake in horizontal line, head infront, tail behind it.

void init_snake(Snake *s, int x, int y)
{
    s->length = 5;
    s->dir = RIGHT;

    // build simple straight snake
    for (int i = 0; i < 5; i++) {
        s->body[i].x = x - i; // each part goes left to CREATE the body
        s->body[i].y = y; // makes sure same row in straight line
    }
}


//Laiba.void move_snake: vvvvv
// moves the snake forward incrementing by 1 step, back of snake body copies the one in front.
//then, the head moves forward based on the current direction.
    
void move_snake(Snake *s)
{
    // important! make the tail follow body. tail behind body front
    for (int i = s->length - 1; i > 0; i--) {
        s->body[i] = s->body[i - 1];
    }

// Laiba. important! move the head based on the direction
// ONLY the head changes the position directly.
    if (s->dir == RIGHT) s->body[0].x++;
    else if (s->dir == LEFT) s->body[0].x--;
    else if (s->dir == UP) s->body[0].y--;
    else if (s->dir == DOWN) s->body[0].y++;
}

// Laiba. change_direction- changes which way snake is going  when USER press arrow keys
// user press ARROWKEYS, then snake moves.
// also blocking snake from crashing into itself.
void change_direction(Snake *s, Direction newDir)
{
    // doesnt allow snake to crash into itself.
    // for ex, if moving right, cant go left rightaway.
    if ((s->dir == LEFT && newDir == RIGHT) ||
        (s->dir == RIGHT && newDir == LEFT) ||
        (s->dir == UP && newDir == DOWN) ||
        (s->dir == DOWN && newDir == UP)) {
        return;
    }

    s->dir = newDir;
}


//Laiba. self_collision- checks if the snake hits ITSELF
// if head is body we check if head matches any other body part
//if YES, then game is over!!!
int self_collision(Snake *s)
{
    for (int i = 1; i < s->length; i++) {
        if (s->body[0].x == s->body[i].x &&
            s->body[0].y == s->body[i].y) {
            return 1; // crash into itself
        }
    }
    return 0; // snake is safe!
}

// Jason 
// Uses the structure to group trophy data
struct Trophy {
    Point pos;      
    int value;      
    time_t spawn_time; 
    int expiry;     
    int active;     
} current_trophy;

//Jason
//Drawing the visible border required

void draw_border(int max_y, int max_x) {
    for (int i = 0; i < max_x; i++) {
        mvaddch(0, i, '#');             
        mvaddch(max_y - 1, i, '#');     
    }
    for (int i = 0; i < max_y; i++) {
        mvaddch(i, 0, '#');             
        mvaddch(i, max_x - 1, '#');     
    }
}

int main() {
    int max_x, max_y;
    Snake mySnake; 

    // Start ncurses
    initscr(); 
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); 
    
    // FORCE the terminal to report the actual maximized size
    endwin();       
    refresh();      
    initscr();      
    
    getmaxyx(stdscr, max_y, max_x); 
    init_snake(&mySnake, max_x / 2, max_y / 2);

    while (1) {
        erase(); 
        
        // This function now uses the full max_y and max_x captured above
        draw_border(max_y, max_x); 

        for (int i = 0; i < mySnake.length; i++) {
            mvaddch(mySnake.body[i].y, mySnake.body[i].x, '*');
        }

        int ch = getch();
        if (ch == KEY_UP) change_direction(&mySnake, UP);
        else if (ch == KEY_DOWN) change_direction(&mySnake, DOWN);
        else if (ch == KEY_LEFT) change_direction(&mySnake, LEFT);
        else if (ch == KEY_RIGHT) change_direction(&mySnake, RIGHT);
        else if (ch == 'q') break; 

        move_snake(&mySnake); 

        // Death logic
        if (mySnake.body[0].x <= 0 || mySnake.body[0].x >= max_x - 1 ||
            mySnake.body[0].y <= 0 || mySnake.body[0].y >= max_y - 1 || 
            self_collision(&mySnake)) {
            break; 
        }

        refresh();
        usleep(100000); 
    }

   endwin(); 
    printf("Game Over! %d x %d\n", max_x, max_y);
    return 0;
}
