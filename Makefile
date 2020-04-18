CC = gcc
CFLAGS = -Wall
INCLUDES = -I./include
LFLAGS = -L./lib
LIBS = -lengine -lGL -lGLEW -lX11 -lm
SRCS = game.c
OBJS = $(SRCS:.c=.o)
MAIN = game

.PHONY: depend clean

all: $(MAIN)
	@echo Game has been compiled

debug: CFLAGS += -DDEBUG -g
debug: all

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS) -Wl,-rpath=./lib
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
clean:
	$(RM) *.o *~ $(MAIN)
