#CC=gcc
CFLAGS   = -Wall -Wextra -mtune=native -no-pie `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm

.SUFFIXES:
.SUFFIXES: .c .o

srcdir	 =src/
BinDir	 =./
TARGETS	 = byte_drawer bit_ops

.PHONY: all
all: $(TARGETS)

byte_drawer: $(srcdir)helper.c $(srcdir)byte_drawer.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)
bit_ops: $(srcdir)helper.c $(srcdir)bit_ops.c
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)


.PHONY: clean
clean:
	@rm $(TARGETS) $(BinDir)byte_drawer data 2>/dev/null || true
