#include <stdio.h>
#include <stdlib.h>
#include "game.h"

void displayMenu() {
    printf("====================\n");
    printf("    SNAKE GAME     \n");
    printf("====================\n");
    printf("1. New Game\n");
    printf("2. Join Game\n");
    printf("3. Continue\n");
    printf("4. Exit\n");
    printf("====================\n");
    printf("Choose an option: ");
}

void startNewGame(int gameMode, int worldType) {
    runGame(gameMode, worldType);
}

void joinGame() {
    printf("Joining game....\n");
}

void continueGame() {
    printf("Continue game...\n");
}

int gameModeSelection() {
    int decision;
    printf("Select game mode:\n");
    printf("1. Standard -> Game ends if nobody joins game for 10 seconds\n");
    printf("2. Timer -> Game ends when time passes. Time is defined before the game.\n");

    scanf("%d", &decision);
    if (decision != 1 && decision != 2) {
        printf("Invalid input! Defaulting to Standard mode.\n");
        decision = 1;
    }
    return decision;
}

int worldTypeSelection() {
    int decision;

    printf("Select game world:\n");
    printf("1. World without obstacles\n");
    printf("2. World with obstacles\n");

    scanf("%d", &decision);
    if (decision != 1 && decision != 2) {
        printf("Invalid input! Defaulting to World without obstacles.\n");
        decision = 1;
    }
    return decision;
}

int mainMenu() {
    int choice;
    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int gameMode =  gameModeSelection();
                int worldType = worldTypeSelection();
                startNewGame(gameMode, worldType);
                break;
              }
            case 2:
                joinGame();
                break;
            case 3:
                continueGame();
                break;
            case 4:
                printf("Exiting game. Goodbye!\n");
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}

