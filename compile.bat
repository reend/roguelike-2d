@echo off
echo Compiling Roguelike RPG...

REM Delete old executable if exists
if exist roguelike.exe (
    echo Deleting old roguelike.exe...
    del roguelike.exe
)

REM Method 1: Working raylib path (C:/raylib/raylib/src)
echo Compiling with raylib...
gcc main.c -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lwinmm -lgdi32 -lopengl32 -o roguelike.exe
if exist roguelike.exe (
    echo Success! roguelike.exe created.
    echo.
    echo Starting game...
    start roguelike.exe
    goto :end
)

REM Method 2: Global raylib installation
echo Trying global raylib...
gcc main.c -lraylib -lwinmm -lgdi32 -lopengl32 -o roguelike.exe 2>nul
if exist roguelike.exe (
    echo Success! roguelike.exe created.
    echo Starting game...
    start roguelike.exe
    goto :end
)

REM Method 3: Local raylib folder
echo Trying local raylib folder...
gcc main.c -I./raylib/include -L./raylib/lib -lraylib -lwinmm -lgdi32 -lopengl32 -o roguelike.exe 2>nul
if exist roguelike.exe (
    echo Success! roguelike.exe created.
    echo Starting game...
    start roguelike.exe
    goto :end
)

echo Error: Could not compile. Please check:
echo 1. Is raylib properly installed?
echo 2. Are the include and library paths correct?
echo 3. Try using Visual Studio with tower_defense.vcxproj instead
echo.
echo For raylib installation, visit: https://github.com/raysan5/raylib/releases

:end
pause
