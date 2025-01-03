#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <pthread.h>


struct termios originalTermios;

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &originalTermios);
    raw = originalTermios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); 
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios); 
}

void initializeWorld(World* world, int width, int height, bool hasObstacles) {
    world->width = width;
    world->height = height;
    world->hasObstacles = hasObstacles;

    world->snake.parts = malloc(sizeof(SnakePart) * 10);
    world->snake.length = 1;
    world->snake.maxLength = 10;
    world->snake.parts[0].x = width / 2;
    world->snake.parts[0].y = height / 2;
    world->snake.dir = RIGHT;

    spawnFood(world);
}

void destroyWorld(World* world) {
    free(world->snake.parts);
    world->snake.parts = NULL;
    world->snake.length = 0;
    world->snake.maxLength = 0;
}

void moveSnake(World* world) {
    Snake* snake = &world->snake;

    for (int i = snake->length - 1; i > 0; i--) {
        snake->parts[i] = snake->parts[i - 1];
    }

    switch (snake->dir) {
        case UP:    snake->parts[0].y--; break;
        case DOWN:  snake->parts[0].y++; break;
        case LEFT:  snake->parts[0].x--; break;
        case RIGHT: snake->parts[0].x++; break;
    }

    if (!world->hasObstacles) {
        if (snake->parts[0].x < 0) snake->parts[0].x = world->width - 1;
        if (snake->parts[0].x >= world->width) snake->parts[0].x = 0;
        if (snake->parts[0].y < 0) snake->parts[0].y = world->height - 1;
        if (snake->parts[0].y >= world->height) snake->parts[0].y = 0;
    }
}

void* handleInput(void* arg) {
    World* world = (World*)arg;
    char input;

    while (1) {
      if (read(STDIN_FILENO, &input, 1) == 1) {
        switch (input) {
            case 'w':
                if (world->snake.dir != DOWN) world->snake.dir = UP;
                break;
            case 's':
                if (world->snake.dir != UP) world->snake.dir = DOWN;
                break;
            case 'a':
                if (world->snake.dir != RIGHT) world->snake.dir = LEFT;
                break;
            case 'd':
                if (world->snake.dir != LEFT) world->snake.dir = RIGHT;
                break;
            case 'q':
                printf("Exiting game...\n");
                exit(0); // Ukončenie hry
            }
        }
    }

    return NULL;
}

bool checkCollision(World* world) {
    Snake* snake = &world->snake;

    if (world->hasObstacles) {
        if (snake->parts[0].x < 0 || snake->parts[0].x >= world->width ||
            snake->parts[0].y < 0 || snake->parts[0].y >= world->height) {
            return true;
        }
    }

    for (int i = 1; i < snake->length; i++) {
        if (snake->parts[0].x == snake->parts[i].x && snake->parts[0].y == snake->parts[i].y) {
            return true;
        }
    }

    return false;
}

bool isTimeUp(World* world) {
    if (world->timeLimit == 0) {
        return false; 
    }

    time_t currentTime = time(NULL);
    return (currentTime - world->startTime) >= world->timeLimit;
}

void spawnFood(World* world) {
    int valid = 0;

    while (!valid) {
        world->food.x = rand() % world->width;
        world->food.y = rand() % world->height;
        valid = 1;

        for (int i = 0; i < world->snake.length; i++) {
            if (world->snake.parts[i].x == world->food.x && world->snake.parts[i].y == world->food.y) {
                valid = 0;
                break;
            }
        }
    }
}

int calculateScore(World* world) {
    return (world->snake.length - 1) * 10;
}

void drawWorld(World* world) {
    system("clear");
    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            if (x == world->food.x && y == world->food.y) {
                printf("+");
            } else {
                int isSnake = 0;
                for (int i = 0; i < world->snake.length; i++) {
                    if (world->snake.parts[i].x == x && world->snake.parts[i].y == y) {
                        printf(i == 0 ? "@" : "o");
                        isSnake = 1;
                        break;
                    }
                }
                if (!isSnake) {
                    if (x == 0 || x == world->width - 1 || y == 0 || y == world->height - 1) {
                        printf("#");
                    } else {
                        printf(" ");
                    }
                }
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", calculateScore(world));
}

void runGame(int gameMode, int worldType) {
    World world;

    if (gameMode == 2) {
        printf("Set your time limit (in seconds):\n");
        scanf("%d", &world.timeLimit);
    } else {
        world.timeLimit = 0; // No time limit
    }

    initializeWorld(&world, 20, 10, worldType == 2);
    world.startTime = time(NULL);

    enableRawMode(); 

    pthread_t inputThread;
    pthread_create(&inputThread, NULL, handleInput, &world);

    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 200000000; 

    while (1) {
        moveSnake(&world);

        if (checkCollision(&world)) {
            printf("Game Over! Final Score: %d\n", calculateScore(&world));
            break;
        }

        if (gameMode == 2 && isTimeUp(&world)) {
            printf("Time's up! Final Score: %d\n", calculateScore(&world));
            break;
        }

        if (world.snake.parts[0].x == world.food.x && world.snake.parts[0].y == world.food.y) {
            world.snake.length++;
            if (world.snake.length > world.snake.maxLength) {
                world.snake.maxLength *= 2;
                world.snake.parts = realloc(world.snake.parts, sizeof(SnakePart) * world.snake.maxLength);
            }
            spawnFood(&world);
        }
        drawWorld(&world);
        nanosleep(&delay, NULL);
    }
    pthread_join(inputThread, NULL);
    disableRawMode();
    destroyWorld(&world);
}


