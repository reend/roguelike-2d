CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
INCLUDES = -I"C:/raylib/raylib/src" -Isrc
LIBS = -L"C:/raylib/raylib/src" -lraylib -lwinmm -lgdi32 -lopengl32
TARGET = roguelike.exe

SRCDIR = src
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/game_state.c $(SRCDIR)/player.c $(SRCDIR)/enemy.c \
          $(SRCDIR)/dungeon.c $(SRCDIR)/renderer.c $(SRCDIR)/resources.c $(SRCDIR)/player_stats.c \
          $(SRCDIR)/render_helpers.c $(SRCDIR)/dungeon_helpers.c $(SRCDIR)/hero_selection.c

OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	del /Q src\*.o $(TARGET) 2>nul || true

rebuild: clean all

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run
