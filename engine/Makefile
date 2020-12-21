CC = gcc
CLFAGS = -Wall
LIBS = -lGL -lGLEW -lX11 -lm
SRCS = engine.c engine_time.c event.c imgload.c proj.c shader.c state.c vec.c util.c window.c
OBJS = $(SRCS:.c=.o)
NAME = libengine
MAIN = $(NAME).so.1.0

.PHONY: depend clean

all: $(MAIN)
	@echo Engine compiled successfully

debug: CFGLAGS += -DDEBUG -g
debug: all

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$(NAME).so.1 -o $(MAIN) $(OBJS) $(LIBS)
.c.o:
	$(CC) $(CFLAGS) -fPIC -c $< -o $@
clean:
	$(RM) *.o *~ $(MAIN)
