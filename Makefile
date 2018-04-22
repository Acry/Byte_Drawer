#CC=gcc
CFLAGS   = -Wall -Wextra -mtune=native `sdl2-config --cflags`
LDFLAGS  = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm

SourceDir=src/
BinDir=./

debug:$(SourceDir)helper.c $(SourceDir)main.c
	$(CC) $(CFLAGS) -g $? -o byte_drawer $(LDFLAGS)
stable:$(SourceDir)helper.c $(SourceDir)main.c
	$(CC) $(CFLAGS) $? -o $(BinDir)byte_drawer $(LDFLAGS)
clean:
	rm -vfr *~ $(BinDir)byte_drawer data
