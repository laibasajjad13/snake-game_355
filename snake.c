#include <stdio.h>

#define MAX_SNAKE 100

// directions for the snake basic movement.
// moving up down left and right.
//using enum instead of words for comparing code, easier
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

// each part of the snake is x y, its a point on the screen.
typedef struct {
    int x;
    int y;
} Point;

// this is the full snake info body- all snake parts
// length- how big/long the snake is currenty
//dir- shows which movement or way snake goes in. left right, etc.
typedef struct {
    Point body[MAX_SNAKE];
    int length;
    Direction dir;
} Snake;

// void init_snake: vvvvv
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


//void move_snake: vvvvv
// moves the snake forward incrementing by 1 step, back of snake body copies the one in front.
//then, the head moves forward based on the current direction.
   
void move_snake(Snake *s)
{
    // important! make the tail follow body. tail behind body front
    for (int i = s->length - 1; i > 0; i--) {
        s->body[i] = s->body[i - 1];
    }

// important! move the head based on the direction
// ONLY the head changes the position directly.
    if (s->dir == RIGHT) s->body[0].x++;
    else if (s->dir == LEFT) s->body[0].x--;
    else if (s->dir == UP) s->body[0].y--;
    else if (s->dir == DOWN) s->body[0].y++;
}

// change_direction- changes which way snake is going  when USER press arrow keys
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


//self_collision- checks if the snake hits ITSELF
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
