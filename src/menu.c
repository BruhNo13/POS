#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ncurses.h>

#define MAPS_DIR "src/maps/"

int display_menu() {
    int choice = 0;
    while (1) {
        clear();
        mvprintw(0, 0, "=== Snake Game Menu ===");
        mvprintw(2, 0, "1. New Game");
        mvprintw(3, 0, "2. Exit");
        mvprintw(5, 0, "Select an option: ");
        refresh();

        int ch = getch();
        if (ch == '1' || ch == '2') {
            choice = ch - '0';
            break;
        }
    }
    return choice;
}

int select_game_mode() {
    int mode = 0;
    while (1) {
        clear();
        mvprintw(0, 0, "=== Select Game Mode ===");
        mvprintw(2, 0, "1. Standard Mode");
        mvprintw(3, 0, "2. Timed Mode");
        mvprintw(5, 0, "Select an option: ");
        refresh();

        int ch = getch();
        if (ch == '1' || ch == '2') {
            mode = ch - '0';
            break;
        }
    }
    return mode;
}

int select_world_type() {
    int world_type = 0;
    while (1) {
        clear();
        mvprintw(0, 0, "=== Select World Type ===");
        mvprintw(2, 0, "1. World without obstacles");
        mvprintw(3, 0, "2. World with obstacles");
        mvprintw(5, 0, "Select an option: ");
        refresh();

        int ch = getch();
        if (ch == '1' || ch == '2') {
            world_type = ch - '0';
            break;
        }
    }
    return world_type;
}

void get_game_time(int *game_time) {
    *game_time = 0;
    while (1) {
        clear();
        mvprintw(0, 0, "=== Set Game Time ===");
        mvprintw(2, 0, "Enter time in seconds: ");
        echo();
        refresh();

        char input[10];
        getstr(input);
        noecho();
        *game_time = atoi(input);

        if (*game_time > 0) {
            break;
        } else {
            mvprintw(4, 0, "Invalid input. Please enter a positive number.");
            refresh();
            getch();
        }
    }
}

void select_world_map(char *selected_map) {
    DIR *dir;
    struct dirent *entry;
    char map_names[100][MAX_MAP_NAME_LENGTH];
    int map_count = 0;

    dir = opendir(MAPS_DIR);
    if (dir == NULL) {
        perror("Failed to open maps directory");
        strcpy(selected_map, "");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt")) {
            strncpy(map_names[map_count], entry->d_name, MAX_MAP_NAME_LENGTH);
            map_names[map_count][MAX_MAP_NAME_LENGTH - 1] = '\0';
            map_count++;
        }
    }
    closedir(dir);

    if (map_count == 0) {
        mvprintw(0, 0, "No maps found in directory.");
        refresh();
        getch();
        strcpy(selected_map, "");
        return;
    }

    int selected_index = 0;

    while (1) {
        clear();
        mvprintw(0, 0, "=== Select World Map ===");
        for (int i = 0; i < map_count; i++) {
            if (i == selected_index) {
                attron(A_UNDERLINE);
                mvprintw(2 + i, 0, "%s", map_names[i]);
                attroff(A_UNDERLINE);
            } else {
                mvprintw(2 + i, 0, "%s", map_names[i]);
            }
        }
        refresh();

        int ch = getch();
        if (ch == KEY_UP && selected_index > 0) {
            selected_index--;
        } else if (ch == KEY_DOWN && selected_index < map_count - 1) {
            selected_index++;
        } else if (ch == '\n') { // Enter key
            strncpy(selected_map, map_names[selected_index], MAX_MAP_NAME_LENGTH);
            selected_map[MAX_MAP_NAME_LENGTH - 1] = '\0';
            break;
        }
    }
}

void get_map_dimensions(int *width, int *height) {
    clear();
    mvprintw(0, 0, "Enter map width: ");
    echo();
    scanw("%d", width);
    mvprintw(1, 0, "Enter map height: ");
    scanw("%d", height);
    noecho();
    refresh();
}

