@echo off
echo ==============================================
echo  Downloading and Setting Up Raylib for MinGW...
echo ==============================================

if not exist "raylib" (
    echo Downloading Raylib 5.0 for MinGW 64-bit...
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip' -OutFile 'raylib.zip'"
    
    echo Extracting Raylib...
    powershell -Command "Expand-Archive -Path 'raylib.zip' -DestinationPath '.'"
    
    echo Renaming folder...
    ren raylib-5.0_win64_mingw-w64 raylib
    
    echo Cleaning up...
    del raylib.zip
) else (
    echo Raylib is already downloaded!
)

echo.
echo ==============================================
echo  Compiling Car_Raylib.cpp...
echo ==============================================

g++ Car_Raylib.cpp -o Car_Raylib.exe -O1 -Wall -I raylib/include -L raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation Successful! 
    echo Running Car_Raylib.exe...
    echo.
    Car_Raylib.exe
) else (
    echo.
    echo Compilation Failed. Please make sure MinGW (g++) is installed and added to your system PATH.
)
pause
