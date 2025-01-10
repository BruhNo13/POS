CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lncurses

SRC_DIR = src
MAPS_DIR = $(SRC_DIR)/maps
SOURCES = $(SRC_DIR)/client.c $(SRC_DIR)/server.c $(SRC_DIR)/world.c $(SRC_DIR)/menu.c
HEADERS = $(SRC_DIR)/world.h $(SRC_DIR)/menu.h

SERVER_BIN = server
CLIENT_BIN = client

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SRC_DIR)/server.c $(SRC_DIR)/world.c
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/server.c $(SRC_DIR)/world.c $(LDFLAGS)

$(CLIENT_BIN): $(SRC_DIR)/client.c $(SRC_DIR)/world.c $(SRC_DIR)/menu.c
	$(CC) $(CFLAGS) -o $@ $(SRC_DIR)/client.c $(SRC_DIR)/world.c $(SRC_DIR)/menu.c $(LDFLAGS)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

