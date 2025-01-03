
CC = gcc
CFLAGS = -Wall -lpthread

SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/game.c $(SRC_DIR)/menu.c
OBJ = $(SRC:.c=.o)
TARGET = game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)/*.o $(TARGET)

