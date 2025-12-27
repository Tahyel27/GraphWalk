#pragma once

#include "raylib.h"
#include "Graph.hpp"
#include "GUI_GraphEditor.hpp"
#include "GUI_Interface.hpp"
#include "MCSimulation.hpp"
#include "PostProcessor.hpp"
#include "Utilities.hpp"

#include <iostream>
#include <queue>
#include <tuple>
#include <memory>

class GUI
{
private:
    const int screenWidth = 1800;
    const int screenHeight = 1000;

    GraphEditor editor;
    
    void loop();

    void runSimulation();

    std::queue<KeyEvent> getKeyEvents();

    std::queue<MouseEvent> getMouseEvent();

    void handleKeyEvents(const KeyEvent &ev);

    void handleMouseEvents(std::queue<MouseEvent> evQueue);
public:
    GUI(/* args */);

    ~GUI();

    int start();
};