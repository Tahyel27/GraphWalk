#pragma once

#include "raylib.h"
#include "Graph.hpp"
#include "GUI_Interface.hpp"

#include <memory>
#include <optional>
#include <iostream>
#include <stack>

class GraphEditor
{
private:
    Graph graph;
    std::stack<std::tuple<Graph, GraphCoordinates>> history;

    GraphCoordinates coords;

    enum class State
    {
        IDLE,
        DRAWING,
        PRE_MOVE,
        MOVING,
    };

    struct Node
    {
        int index;
        int cellx;
        int celly;
    };

    std::optional<Node> markedNode;

    State editState;

    bool isMoving = false;

    bool TESFLAG = false;
    int OFF_X = 900;
    int OFF_Y = 500;
    float SCALE = 250;
    float RADIUS = 10;

    void drawCellNodes(int x, int y);

    void drawCellEdges(int x, int y);

    void drawUnitCell();

    void handleLeftClick(MouseEvent event);

    void handleRightClick(MouseEvent event);

    void handleMouseMovements();

    Vector2 vec2ToScreen(Vector2 v);

    Vector2 screenToCoords(Vector2 screen);

    Vector2 applySkew(Vector2 v);

    std::tuple<float, float> getNodeCoordinates(const GraphCoordinates &data, int node, int i, int j);

public:
    GraphEditor(/* args */);

    void setGraph(Graph graph_, GraphCoordinates coords_);

    std::tuple<Graph, GraphCoordinates> getGraph();

    std::tuple<int, int, int> getStartingNode();

    void handleKeyEvent(KeyEvent event);
    void handleMouseEvent(MouseEvent event);

    void draw();

    ~GraphEditor();
};