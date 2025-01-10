#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "world.h"

#define PORT 1239
#define BUFFER_SIZE 2048
#define MAX_CLIENTS 1
#define MAX_MAP_NAME_LENGTH 100

int running = 1;
pthread_t move_thread;

void handle_client_input(World *world, const char *input) {
    if (strcmp(input, "UP") == 0 && world->snake.direction != 2) world->snake.direction = 0;
    else if (strcmp(input, "RIGHT") == 0 && world->snake.direction != 3) world->snake.direction = 1;
    else if (strcmp(input, "DOWN") == 0 && world->snake.direction != 0) world->snake.direction = 2;
    else if (strcmp(input, "LEFT") == 0 && world->snake.direction != 1) world->snake.direction = 3;
}

void send_game_world(int client_fd, World *world) {
    if (send(client_fd, world, sizeof(World), 0) <= 0) {
        perror("Failed to send game world");
    }
}

void *auto_move_snake(void *arg) {
    void **args = (void **)arg;
    World *world = (World *)args[0];
    int client_fd = *(int *)args[1];
    int world_type = *(int *)args[2];

    while (running) {
        if (move_snake(world, world_type) == -1) {
            printf("\nGame Over!\n");
            running = 0;
            break;
        }
        send_game_world(client_fd, world); 
        usleep(300000); 
    }

    return NULL;
}

int main() {
    srand(time(NULL));
    World world;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("\nServer is listening on port %d...\n", PORT);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_fd, buffer, BUFFER_SIZE, 0);
    int mode, world_type, map_width, map_height;
    char selected_map[MAX_MAP_NAME_LENGTH] = "";
    sscanf(buffer, "MODE %d\nWORLD %d\nMAP %s\nDIMENSIONS %d %d\n", &mode, &world_type, selected_map, &map_width, &map_height);
    printf("Received settings: Mode=%d, World_type=%d, Width=%d, Height=%d, Map=%s\n", mode, world_type, map_width, map_height, selected_map);

    if (world_type == 1) {
        init_world(&world, &world.snake, world_type, map_width, map_height, selected_map);
    } else {
        if (load_world_from_file(selected_map, &world) != 0) {
            printf("Failed to load world map.\n");
            close(client_fd);
            close(server_fd);
            exit(EXIT_FAILURE);
        }
    }

    send_game_world(client_fd, &world);

    void *args[3] = {&world, &client_fd, &world_type};
    pthread_create(&move_thread, NULL, auto_move_snake, &args);

    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            running = 0;
            break;
        }

        printf("Received from client: %s\n", buffer);
        handle_client_input(&world, buffer);
    }

    pthread_join(move_thread, NULL);
    close(client_fd);
    close(server_fd);

    printf("Server shut down.\n");
    return 0;
}

