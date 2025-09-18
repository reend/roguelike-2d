@echo off
echo Compiling Modular Roguelike RPG...

REM Delete old executable if exists
if exist roguelike.exe (
    echo Deleting old roguelike.exe...
    del roguelike.exe
)

REM Clean old object files
if exist src\*.o (
    echo Cleaning object files...
    del /Q src\*.o
)

REM Method 1: Working raylib path (C:/raylib/raylib/src)
echo Compiling modular source files...
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/main.c -o src/main.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/game_state.c -o src/game_state.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/player.c -o src/player.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/enemy.c -o src/enemy.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/dungeon.c -o src/dungeon.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/renderer.c -o src/renderer.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/resources.c -o src/resources.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/player_stats.c -o src/player_stats.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/render_helpers.c -o src/render_helpers.o
gcc -Wall -Wextra -std=c99 -O2 -I"C:/raylib/raylib/src" -Isrc -c src/dungeon_helpers.c -o src/dungeon_helpers.o

echo Linking...
gcc src/main.o src/game_state.o src/player.o src/enemy.o src/dungeon.o src/renderer.o src/resources.o src/player_stats.o src/render_helpers.o src/dungeon_helpers.o -L"C:/raylib/raylib/src" -lraylib -lwinmm -lgdi32 -lopengl32 -o roguelike.exe

if exist roguelike.exe (
    echo Success! Modular roguelike.exe created.
    echo.
    echo Code structure:
    echo - src/main.c         - Entry point
    echo - src/game_state.c   - Game management
    echo - src/player.c       - Player logic
    echo - src/enemy.c        - Enemy AI
    echo - src/dungeon.c      - Map generation
    echo - src/renderer.c     - Graphics
    echo - src/resources.c    - Asset loading
    echo - src/player_stats.c - Player statistics
    echo - src/render_helpers.c - Rendering utilities
    echo - src/dungeon_helpers.c - Optimized position finding
    echo.
    echo Starting game...
    start roguelike.exe
    goto :end
)

echo Error: Compilation failed
echo Try: make all (if you have make installed)

:end
pause
