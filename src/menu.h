#ifndef MENU_H
#define MENU_H

#define MAX_MAP_NAME_LENGTH 100

int display_menu();
int select_game_mode();
int select_world_type();
void get_game_time(int *game_time);
void select_world_map(char *selected_map);
void get_map_dimensions(int *width, int *height);

#endif 

