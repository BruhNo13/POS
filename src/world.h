#ifndef WORLD_H
#define WORLD_H

#define MAX_MAP_SIZE 100
#define MAX_BODY_LENGTH 100

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position head;
    int length;
    int direction;
    Position body[MAX_BODY_LENGTH];
} Snake;

typedef struct {
    Snake snake;
    int width;
    int height;
    int score;
    char grid[MAX_MAP_SIZE][MAX_MAP_SIZE];
    Position fruit;
} World;

void init_world(World *world, Snake *snake, int world_type, int width, int height, const char *map_filename);
int move_snake(World *world, int world_type);
int load_world_from_file(const char *filename, World *world);
void generate_fruit(World *world);

#endif 

