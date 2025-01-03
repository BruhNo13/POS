#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <time.h>

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    int x, y;
} SnakePart;

typedef struct {
    SnakePart* parts; 
    int length;       
    int maxLength;    
    Direction dir;  
} Snake;

typedef struct {
    int x, y;
} Food;

typedef struct {
    int width;
    int height; 
    Snake snake;
    Food food;
    bool hasObstacles; 
    int timeLimit;
    time_t startTime;
} World;

void initializeWorld(World* world, int width, int height, bool hasObstacles);
void destroyWorld(World* world);
void moveSnake(World* world);
bool checkCollision(World* world);
void* handleInput(void* arg);
bool isTimeUp(World* world);
void spawnFood(World* world);
int calculateScore(World* world);
void drawWorld(World* world);
void runGame(int gameMode, int worldType);

#endif 


