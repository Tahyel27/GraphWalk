#pragma once

#include "raylib.h"

class GUI
{
private:
    const int screenWidth = 1200;
    const int screenHeight = 800;
    
    void loop();
public:
    GUI(/* args */);

    ~GUI();

    int start();
};