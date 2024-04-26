CC=gcc
CFLAGS=-Wall -Wextra -Wformat -pedantic -ggdb
IDIR=include
INCLUDE=-I$(IDIR)/
LIBS= -lSDL2 -lSDL2_ttf
SRCS=main.c $(IDIR)/libattopng.c
OUT=a.out

build:
	$(CC) $(CFLAGS) $(INCLUDE) -o $(OUT) $(SRCS) $(LIBS)

run: build
	./$(OUT)
