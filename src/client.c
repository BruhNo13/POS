#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ncurses.h>
#include <sys/wait.h>
#include <signal.h>
#include "menu.h"
#include "world.h"

#define CONTROL_PORT 1239
#define BUFFER_SIZE 2048
#define MAX_BODY_LENGTH 100
#define MAX_MAP_SIZE 100

typedef struct {
    int mode;
    int world_type;
    int game_time;
    int map_width;
    int map_height;
    char selected_map[MAX_MAP_NAME_LENGTH];
    char grid[MAX_MAP_SIZE][MAX_MAP_SIZE];
} GameSettings;

int sock_fd;
pthread_t input_thread, render_thread;
int running = 1;
int score = 0;
time_t start_time;
pid_t server_pid = -1;

void cleanup() {
    if (server_pid > 0) {
        kill(server_pid, SIGTERM);
        waitpid(server_pid, NULL, 0);
    }
    close(sock_fd);
    endwin();
}

void *handle_input(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];

    while (running) {
        int ch = getch();

        switch (ch) {
            case 'w': strcpy(buffer, "UP"); break;
            case 'd': strcpy(buffer, "RIGHT"); break;
            case 's': strcpy(buffer, "DOWN"); break;
            case 'a': strcpy(buffer, "LEFT"); break;
            case 'q': strcpy(buffer, "QUIT"); running = 0; break;
            default: continue;
        }

        send(sock_fd, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "QUIT") == 0) break;
    }

    return NULL;
}

void render_score_and_time(GameSettings *settings, int elapsed_time) {
    mvprintw(settings->map_height + 2, 0, "Score: %d", score);
    mvprintw(settings->map_height + 3, 0, "Time: %d seconds", elapsed_time);
    refresh();
}

void *receive_and_render(void *arg) {
    GameSettings *settings = (GameSettings *)arg;
    World world; 

    start_time = time(NULL);

    while (running) {
        if (recv(sock_fd, &world, sizeof(World), 0) <= 0) {
            mvprintw(2, 0, "Disconnected from server.");
            refresh();
            running = 0;
            break;
        }

        settings->map_width = world.width;
        settings->map_height = world.height;
        score = world.score;

        memcpy(settings->grid, world.grid, sizeof(world.grid));

        clear();
        if (settings->world_type == 1) {
            for (int y = 0; y < settings->map_height; y++) {
              for (int x = 0; x < settings->map_width;  x++) {
                  mvprintw(y, x, "%c", settings->grid[y][x]);
                }
            }
        } else {
            for (int y = 0; y < settings->map_height; y++) {
                for (int x = 0; x < settings->map_width;  x++) {
                    mvprintw(y, x, "%c", settings->grid[y][x]);
                }
            }
        }

        int elapsed_time;
        if (settings->mode == 2) { // Timed mode
            elapsed_time = settings->game_time - (int)(time(NULL) - start_time);
            if (elapsed_time <= 0) {
                mvprintw(settings->map_height + 4, 0, "Time's up! Ending game.");
                refresh();
                running = 0;
                break;
            }
        } else { 
            elapsed_time = (int)(time(NULL) - start_time);
        }

        render_score_and_time(settings, elapsed_time);
    }

    return NULL;
}

void game_loop() {
    GameSettings settings;

    settings.mode = select_game_mode();
    settings.world_type = select_world_type();

    if (settings.mode == 2) {
        get_game_time(&settings.game_time);
    } else {
        settings.game_time = 0;
    }

    if (settings.world_type == 2) {
        select_world_map(settings.selected_map);
    } else {
        strcpy(settings.selected_map, "None");
        get_map_dimensions(&settings.map_width, &settings.map_height);
    }

    server_pid = fork();
    if (server_pid < 0) {
        perror("Failed to fork server process");
        cleanup();
        exit(EXIT_FAILURE);
    } else if (server_pid == 0) {
        execl("./server", "./server", NULL);
        perror("Failed to exec server");
        exit(EXIT_FAILURE);
    }

    sleep(2); 

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        cleanup();
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CONTROL_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to the server failed");
        cleanup();
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "MODE %d\nWORLD %d\nMAP %s\nDIMENSIONS %d %d\n",
             settings.mode, settings.world_type, settings.selected_map, settings.map_width, settings.map_height);
    send(sock_fd, buffer, strlen(buffer), 0);
    // mvprintw(0, 0, "Connected to the server. Use W/A/S/D to move, Q to quit.");
    refresh();

    running = 1;
    pthread_create(&input_thread, NULL, handle_input, NULL);
    pthread_create(&render_thread, NULL, receive_and_render, &settings);

    pthread_join(input_thread, NULL);
    pthread_join(render_thread, NULL);

    cleanup();
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    while (1) {
        int choice = display_menu();
        if (choice == 1) { 
            game_loop();
        } else if (choice == 2) { 
            mvprintw(9, 0, "Exiting...");
            refresh();
            sleep(1);
            cleanup();
            break;
        }
    }

    cleanup();
    return 0;
}

