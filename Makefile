TARGET_LIB = gconn
TARGET_APP = $(TARGET_LIB)_app


BIN = bin/
LIB = lib/
OBJ = obj/
SRC = src/

AR = ar
CC = gcc
CFLAGS  = -Wall
LDFLAGS = -L$(LIB) -l$(TARGET_LIB) -lcurl

.PHONY: all clean init

all: init $(TARGET_LIB) $(TARGET_APP)

init:
	mkdir -p $(BIN) $(LIB) $(OBJ)

median.o: $(SRC)median.c
	$(CC) $(CFLAGS) -c $(SRC)median.c -o $(OBJ)median.o

$(TARGET_LIB): $(SRC)$(TARGET_LIB).c median.o
	$(CC) $(CFLAGS) -c $(SRC)$(TARGET_LIB).c -o $(OBJ)$(TARGET_LIB).o
	$(AR) rcs $(LIB)lib$(TARGET_LIB).a $(OBJ)$(TARGET_LIB).o $(OBJ)median.o

$(TARGET_APP): $(SRC)$(TARGET_APP).c
	$(CC) $(CFLAGS) $(SRC)$(TARGET_APP).c -o $(BIN)$(TARGET_APP) $(LDFLAGS)

clean:
	rm -rf $(OBJ) $(LIB) $(BIN)
