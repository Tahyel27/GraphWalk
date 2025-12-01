#include "GUI.hpp"

GUI::GUI()
{

}

GUI::~GUI()
{

}

void GUI::loop()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    EndDrawing();
}

int GUI::start()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "GraphWalk");

    while (!WindowShouldClose())
    {
        loop();
    }
        
    CloseWindow();

    return 0;
}
