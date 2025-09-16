@echo off
echo Compiling Tower Defense...

REM Try different compilation methods

REM Method 1: Global raylib installation
echo Trying global raylib...
gcc main.c -lraylib -lwinmm -lgdi32 -lopengl32 -o tower_defense.exe 2>nul
if exist tower_defense.exe (
    echo Success! tower_defense.exe created.
    goto :end
)

REM Method 2: Local raylib folder
echo Trying local raylib folder...
gcc main.c -I./raylib/include -L./raylib/lib -lraylib -lwinmm -lgdi32 -lopengl32 -o tower_defense.exe 2>nul
if exist tower_defense.exe (
    echo Success! tower_defense.exe created.
    goto :end
)

REM Method 3: Common raylib locations
echo Trying common raylib locations...
gcc main.c -I"C:/raylib/raylib/src" -L"C:/raylib/raylib/src" -lraylib -lwinmm -lgdi32 -lopengl32 -o tower_defense.exe 2>nul
if exist tower_defense.exe (
    echo Success! tower_defense.exe created.
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
