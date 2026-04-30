#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h> 

// Jason
// Global variables for game features
// MAX_SNAKE set to 1000 to support half-perimeter win condition
#define MAX_SNAKE 1000
int pending_growth = 0; 

// Laiba. directions for the snake basic movement.
// moving up down left and right.
// using enum instead of words for comparing code, easier
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
// dir- shows which movement or way snake goes in. left right, etc.
typedef struct {
    Point body[MAX_SNAKE];
    int length;
    Direction dir;
} Snake;

// Jason: Game Rules / Mechanics
// Uses the structure to group trophy data
struct Trophy {
    Point pos;      
    int value;      
    time_t spawn_time; 
    int expiry;     
    int active;     
} current_trophy;

// Laiba: Snake Logic - Snake initialization (length & direction)
// creates the snake at the start
// startX and startY- WHERE the snake starts on the screen
// Initial length- set as 5. (starting size of our snake)
// the snake will naturally go right as default. (Ayele: Randomized for project reqs)
// building snake in horizontal line, head infront, tail behind it.
void init_snake(Snake *s, int x, int y)
{
    s->length = 5;
    s->dir = (Direction)(rand() % 4); // Ayele: Random start direction

    // build simple straight snake
    for (int i = 0; i < s->length; i++) {
        // Ayele: Adjusted spacing for higher horizontal speed
        s->body[i].x = x - (i * 3); 
        s->body[i].y = y; // makes sure same row in straight line
    }
}

// Laiba: Snake Logic - Snake movement system
// moves the snake forward incrementing by 1 step, back of snake body copies the one in front.
// then, the head moves forward based on the current direction.
// Jason: Game Rules / Mechanics - Snake growth logic
void move_snake(Snake *s)
{
    // Jason: Growth logic implementation
    // If snake ate something, we increase length by NOT shifting the tail off
    if (pending_growth > 0) {
        if (s->length < MAX_SNAKE) {
            s->length++;
            pending_growth--;
        }
    }

    // important! make the tail follow body. tail behind body front
    for (int i = s->length - 1; i > 0; i--) {
        s->body[i] = s->body[i - 1];
    }

    // Laiba. important! move the head based on the direction
    // ONLY the head changes the position directly.
    // Speed Boost: Moved to 3 units for LEFT/RIGHT to make it move way faster
    if (s->dir == RIGHT) s->body[0].x += 3;      
    else if (s->dir == LEFT) s->body[0].x -= 3;   
    else if (s->dir == UP) s->body[0].y--;
    else if (s->dir == DOWN) s->body[0].y++;
}

// Laiba: Snake Logic - Direction control (arrow keys)
// change_direction- changes which way snake is going when USER press arrow keys
void change_direction(Snake *s, Direction newDir)
{
    s->dir = newDir;
}

// Laiba: Snake Logic - Self-collision detection
// self_collision- checks if the snake hits ITSELF
// if head is body we check if head matches any other body part
// if YES, then game is over!!!
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

// Jason: Game Rules / Mechanics - Trophy generation (1-9 values)
// A trophy is shown at a random location with a value and expiry
void spawn_trophy(int max_y, int max_x) {
    // Ayele: Keeping trophy slightly away from borders for the fast snake
    current_trophy.pos.x = (rand() % (max_x - 6)) + 3;
    current_trophy.pos.y = (rand() % (max_y - 2)) + 1;
    current_trophy.value = (rand() % 9) + 1; // 1-9 value
    current_trophy.expiry = (rand() % 9) + 1; // 1-9 second expiry
    current_trophy.spawn_time = time(NULL);
    current_trophy.active = 1;
}

// Ayele: UI / ncurses Display - Snake pit (game border & terminal setup)
// Drawing the visible border required
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

// Ayele: UI / ncurses Display - Terminal Setup
int main() {
    int max_x, max_y;
    Snake mySnake; 
    srand(time(NULL));

    // Start ncurses
    initscr(); 
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); 
    
    // FORCE the terminal to report the actual maximized size
    getmaxyx(stdscr, max_y, max_x); 
    init_snake(&mySnake, max_x / 2, max_y / 2);
    
    // Jason: Initial trophy spawn
    spawn_trophy(max_y, max_x);

    while (1) {
        erase(); 
        
        // Ayele: Drawing the snake pit border
        draw_border(max_y, max_x); 

        // Jason: Game Rules / Mechanics - Trophy expiration timer
        if (time(NULL) - current_trophy.spawn_time >= current_trophy.expiry) {
            spawn_trophy(max_y, max_x);
        }

        // Ayele: UI / ncurses Display - Trophy rendering
        mvprintw(current_trophy.pos.y, current_trophy.pos.x, "%d", current_trophy.value);

        // Ayele: UI / ncurses Display - Drawing snake on screen
        for (int i = 0; i < mySnake.length; i++) {
            mvaddch(mySnake.body[i].y, mySnake.body[i].x, '*');
        }

        int ch = getch();
        Direction oldDir = mySnake.dir;

        if (ch == KEY_UP) change_direction(&mySnake, UP);
        else if (ch == KEY_DOWN) change_direction(&mySnake, DOWN);
        else if (ch == KEY_LEFT) change_direction(&mySnake, LEFT);
        else if (ch == KEY_RIGHT) change_direction(&mySnake, RIGHT);
        else if (ch == 'q') break; 

        // Laiba: Snake Logic - Direction control (no reverse rule)
        // doesn't allow snake to crash into itself.
        // Ayele: Rule check - Reversing direction results in death
        if ((oldDir == UP && mySnake.dir == DOWN) || (oldDir == DOWN && mySnake.dir == UP) ||
            (oldDir == LEFT && mySnake.dir == RIGHT) || (oldDir == RIGHT && mySnake.dir == LEFT)) {
            break;
        }

        // Ayele: Real-time movement display
        move_snake(&mySnake); 

        // Jason: Game Rules / Mechanics - Score system & Growth logic
        // Hitbox expanded to 2 for the faster 3-step movement
        if ((abs(mySnake.body[0].x - current_trophy.pos.x) <= 2) && 
            (mySnake.body[0].y == current_trophy.pos.y)) {
            pending_growth += current_trophy.value; // Add trophy value to growth
            spawn_trophy(max_y, max_x);
        }

        // Jason: Game Rules / Mechanics - Win condition (Half perimeter)
        if (mySnake.length >= (max_x + max_y)) {
            endwin();
            printf("You Win! Snake reached half-perimeter length.\n");
            return 0;
        }

        // Laiba: Snake Logic - Wall & Self-collision detection
        if (mySnake.body[0].x <= 0 || mySnake.body[0].x >= max_x - 1 ||
            mySnake.body[0].y <= 0 || mySnake.body[0].y >= max_y - 1 || 
            self_collision(&mySnake)) {
            break; 
        }

        refresh();
        
        // Ayele: Movement timing/speed floor lowered to make game faster overall
        int delay = 90000 - (mySnake.length * 1000);
        if (delay < 30000) delay = 30000; // Speed cap
        usleep(delay); 
    }

    endwin(); 
    printf("Game Over! Final Length: %d\n", mySnake.length);
    return 0;
}
