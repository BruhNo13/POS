#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void init_world(World *world, Snake *snake, int world_type, int width, int height, const char *map_filename) {
    if (world_type == 1) {
        
        world->width = width;
        world->height = height;
        world->score = 0;

        world->snake.head.x = world->width / 2;
        world->snake.head.y = world->height / 2;
        world->snake.length = 1;
        world->snake.direction = 1; 
        world->snake.body[0] = snake->head;

        memset(world->grid, '.', sizeof(world->grid));
        world->grid[snake->head.y][snake->head.x] = 'S';

        generate_fruit(world);
    } else {
        if (load_world_from_file(map_filename, world) != 0) {
            fprintf(stderr, "Failed to load world from file: %s\n", map_filename);
            exit(EXIT_FAILURE);
        }
    }
}

int move_snake(World *world, int world_type) {
    Position next_position = world->snake.body[0];

    switch (world->snake.direction) {
        case 0: next_position.y--; break; 
        case 1: next_position.x++; break; 
        case 2: next_position.y++; break; 
        case 3: next_position.x--; break; 
    }

    if (world_type == 1) {
        if (next_position.x < 0) next_position.x = world->width - 1;
        if (next_position.x >= world->width) next_position.x = 0;
        if (next_position.y < 0) next_position.y = world->height - 1;
        if (next_position.y >= world->height) next_position.y = 0;
    } else {
        if (next_position.x < 0 || next_position.x >= world->width ||
            next_position.y < 0 || next_position.y >= world->height ||
            world->grid[next_position.y][next_position.x] == '#') {
            return -1; 
        }
    }

    for (int i = 0; i < world->snake.length; i++) {
        if (world->snake.body[i].x == next_position.x && world->snake.body[i].y == next_position.y) {
            return -1; 
        }
    }

    if (next_position.x == world->fruit.x && next_position.y == world->fruit.y) {
        world->snake.length++;
        world->score++;
        generate_fruit(world); 
    }

    for (int y = 0; y < world->height; y++) {
        for (int x = 0; x < world->width; x++) {
            if (world->grid[y][x] != '#') {
                world->grid[y][x] = '.'; 
            }
        }
    }

    for (int i = world->snake.length - 1; i > 0; i--) {
        world->snake.body[i] = world->snake.body[i - 1];
    }
    world->snake.body[0] = next_position;

    for (int i = 0; i < world->snake.length; i++) {
        world->grid[world->snake.body[i].y][world->snake.body[i].x] = (i == 0) ? 'S' : '+'; 
    }
    world->grid[world->fruit.y][world->fruit.x] = 'O'; 

    return 0; 
}

void generate_fruit(World *world) {
    int valid_position = 0;

    while (!valid_position) {
        world->fruit.x = rand() % world->width;
        world->fruit.y = rand() % world->height;

        if (world->grid[world->fruit.y][world->fruit.x] == '#' ||
            world->grid[world->fruit.y][world->fruit.x] == 'S' ||
            world->grid[world->fruit.y][world->fruit.x] == '+') {
            continue; 
        }

        valid_position = 1; 
    }
}

int load_world_from_file(const char *filename, World *world) {
    char full_path[128] = "src/maps/";
    strcat(full_path, filename);

    FILE *file = fopen(full_path, "r");
    if (!file) {
        perror("Failed to open world file");
        return -1;
    }

    if (fscanf(file, "%d %d", &world->width, &world->height) != 2) {
        perror("Failed to read world dimensions");
        fclose(file);
        return -1;
    }

    fgetc(file); 

    char line[MAX_MAP_SIZE + 2]; 
    for (int y = 0; y < world->height; y++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            perror("Failed to read map line");
            fclose(file);
            return -1;
        }
        // printf("Loaded row %d: %s\n", y, line);
        for (int x = 0; x < world->width; x++) {
            char cell = line[x];
            world->grid[y][x] = cell;

            if (cell == 'S') {
                world->snake.head.x = x;
                world->snake.head.y = y;
                world->snake.body[0] = world->snake.head;
                world->snake.length = 1;
                world->snake.direction = 1;
            } else if (cell == 'O') {
                world->fruit.x = x;
                world->fruit.y = y;
            }
        }
    }
    generate_fruit(world);
    fclose(file);

    printf("Loaded world with dimensions %d x %d\n", world->width, world->height);
    return 0;
}

