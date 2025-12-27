#include "GUI.hpp"

GUI::GUI()
{

}

GUI::~GUI()
{

}

std::tuple<Graph, GraphCoordinates> getSquareGrid()
{
    Graph graph;
    graph.addNode();
    graph.addBidirectionalEdge(0, 0, 1, 0);
    graph.addBidirectionalEdge(0, 0, 0, 1);
    // graph.addBidirectionalEdge(0,0,0,10);

    GraphCoordinates coords;
    coords.N = 1;
    coords.scaleX = 1;
    coords.scaleY = 1;
    coords.skewX = 0;
    coords.skewY = 0;
    coords.X = {0};
    coords.Y = {0};

    return std::tuple<Graph, GraphCoordinates>(graph, coords);
}

void GUI::loop()
{
    auto keyEvents = getKeyEvents();

    while (!keyEvents.empty())
    {
        handleKeyEvents(keyEvents.front());
        keyEvents.pop();
    }

    auto mouseEvents = getMouseEvent();

    handleMouseEvents(std::move(mouseEvents));

    BeginDrawing();

    ClearBackground(RAYWHITE);

    editor.draw();

    EndDrawing();
}

void GUI::runSimulation()
{
    auto [graph, coords] = editor.getGraph();
    auto [index, cellx, celly] = editor.getStartingNode();

    std::random_device seedgen;
    auto cfg = parseConfigFile();

    auto simulation = MCSimulation(graph, coords, cfg, seedgen());

    simulation.setStartingPosition(index, cellx, celly);

    std::cout << "Starting simulation for " << simulation.getRunCount() << " runs " << simulation.getStepCount() << " steps..." << std::endl;

    simulation.run();

    std::cout << "Simulation done" << std::endl;

    PostProcessor::writeResultsToFile(simulation, coords);

}

std::queue<KeyEvent> GUI::getKeyEvents()
{
    auto queue = std::queue<KeyEvent>();
    for (size_t i = 32; i < 97; i++)
    {
        if(IsKeyPressed(i))
        {
            queue.emplace(KeyEvent{static_cast<KeyboardKey>(i), KeyEvent::Type::PRESSED});
        }
    }

    for (size_t i = 256; i < 337; i++)
    {
        if(IsKeyPressed(i))
        {
            queue.emplace(KeyEvent{static_cast<KeyboardKey>(i), KeyEvent::Type::PRESSED});
        }
    }

    for (size_t i = 32; i < 97; i++)
    {
        if (IsKeyReleased(i))
        {
            queue.emplace(KeyEvent{static_cast<KeyboardKey>(i), KeyEvent::Type::RELEASED});
        }
    }

    for (size_t i = 256; i < 337; i++)
    {
        if (IsKeyReleased(i))
        {
            queue.emplace(KeyEvent{static_cast<KeyboardKey>(i), KeyEvent::Type::RELEASED});
        }
    }

    return std::move(queue);
}

std::queue<MouseEvent> GUI::getMouseEvent()
{
    auto queue = std::queue<MouseEvent>();
    for (size_t i = 0; i < 7; i++)
    {
        if (IsMouseButtonPressed(i))
        {
            queue.emplace(MouseEvent{GetMousePosition(), static_cast<MouseButton>(i), MouseEvent::Type::PRESSED});
        }
        if (IsMouseButtonReleased(i))
        {
            queue.emplace(MouseEvent{GetMousePosition(), static_cast<MouseButton>(i), MouseEvent::Type::RELEASED});
        }        
    }

    auto delta = GetMouseDelta();
    if (delta.x != 0 && delta.y != 0)
    {
        queue.emplace(MouseEvent{GetMousePosition(), MouseButton::MOUSE_BUTTON_LEFT, MouseEvent::Type::MOVED});
    }

    return std::move(queue);
}

void GUI::handleKeyEvents(const KeyEvent &ev)
{
    editor.handleKeyEvent(ev);

    if (ev.key == KEY_R && ev.type == KeyEvent::Type::PRESSED)
    {
        runSimulation();
    }
}

void GUI::handleMouseEvents(std::queue<MouseEvent> evQueue)
{
    while (!evQueue.empty())
    {
        editor.handleMouseEvent(evQueue.front());
        evQueue.pop();
    }
}

int GUI::start()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "GraphWalk");

    SetExitKey(0);

    editor = GraphEditor();
    
    auto [g, c] = getSquareGrid();
    editor.setGraph(g, c);

    while (!WindowShouldClose())
    {
        loop();
    }
        
    CloseWindow();

    return 0;
}
