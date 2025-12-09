#include "GUI_GraphEditor.hpp"

void GraphEditor::drawCellNodes(int x, int y)
{
    int N = graph.getN();

    auto data = graph.getGraphData();

    // draw the nodes
    for (size_t i = 0; i < N; i++)
    {
        auto [xc, yc] = getNodeCoordinates(coords, i, x, y);
        xc = SCALE * xc;
        yc = SCALE * yc;

        if (TESFLAG)
        {
            DrawCircle(static_cast<int>(xc) + OFF_X, static_cast<int>(yc) + OFF_Y, RADIUS, BLUE);
        }
        else
        {
            DrawCircle(static_cast<int>(xc) + OFF_X, static_cast<int>(yc) + OFF_Y, RADIUS, GREEN);
        }
        
    }
}

void GraphEditor::drawCellEdges(int x, int y)
{
    auto data = graph.getGraphData();
    int N = graph.getN();

    // draw the edges
    for (size_t i = 0; i < graph.getEdges(); i++)
    {
        auto [Ax, Ay] = getNodeCoordinates(coords, data.vert_A[i], x, y);
        auto [Bx, By] = getNodeCoordinates(coords, data.vert_B[i], x + data.inc_X[i], y + data.inc_Y[i]);
        auto start = Vector2{SCALE * Ax + OFF_X, SCALE * Ay + OFF_Y};
        auto end = Vector2{SCALE * Bx + OFF_X, SCALE * By + OFF_Y};
        DrawLineEx(start, end, 4, RED);
    }
}

void GraphEditor::drawUnitCell()
{
    float X = SCALE + OFF_X;
    float Y = SCALE + OFF_Y;

    
    auto topright = applySkew(Vector2{0.5,0.5});
    auto topleft = applySkew(Vector2{-0.5,0.5});
    auto bottomleft = applySkew(Vector2{-0.5,-0.5});
    auto bottomright = applySkew(Vector2{0.5,-0.5});

    topright = vec2ToScreen(topright);
    topleft = vec2ToScreen(topleft);
    bottomleft = vec2ToScreen(bottomleft);
    bottomright = vec2ToScreen(bottomright);

    DrawTriangle(topright, bottomleft, topleft, LIGHTGRAY);
    DrawTriangle(bottomright, bottomleft, topright, LIGHTGRAY);
}

void GraphEditor::handleLeftClick(MouseEvent event)
{
    for (int i = -5; i < 6; i++)
    {
        for (int j = -5; j < 6; j++)
        {
            for (int k = 0; k < graph.getN(); k++)
            {
                auto [x, y] = getNodeCoordinates(coords, k, i, j);
                auto mx = event.pos.x; auto my = event.pos.y;
                x = SCALE * x + OFF_X; y = SCALE * y + OFF_Y;
                auto len = (mx - x) * (mx - x) + (my - y) * (my - y);
                if (len < RADIUS * RADIUS)
                {
                    if (markedNode.has_value() && editState == State::DRAWING)
                    {
                        history.emplace(std::tuple<Graph, GraphCoordinates>{graph, coords});
                        graph.addBidirectionalEdge(markedNode.value().index, k, i - markedNode.value().cellx, j - markedNode.value().celly);
                    }
                    markedNode = Node{k, i, j};

                    editState = State::PRE_MOVE;

                    return;
                }

            }
            
        }
        
    }
    if (editState == State::DRAWING)
    {
        editState = State::IDLE;
    }
    else
    {
        markedNode = std::nullopt;
    }
    
}

void GraphEditor::handleRightClick(MouseEvent event)
{
    auto coord = screenToCoords(Vector2{event.pos.x,event.pos.y});
    history.push(std::tuple<Graph, GraphCoordinates>{graph, coords});
    graph.addNode();
    coords.X.push_back(coord.x);
    coords.Y.push_back(coord.y);
    coords.N++;
}


Vector2 GraphEditor::vec2ToScreen(Vector2 v)
{
    return Vector2{SCALE * v.x + OFF_X, SCALE * v.y + OFF_Y};
}

Vector2 GraphEditor::screenToCoords(Vector2 screen)
{
    auto x = (screen.x - OFF_X) / SCALE;
    auto y = (screen.y - OFF_Y) / SCALE;
    auto idet = 1 / (1 + coords.skewX * coords.scaleY);
    auto xc = idet * (x + coords.skewY * y);
    auto yc = idet * (y - coords.skewX * x);
    return Vector2{xc, yc};
}

Vector2 GraphEditor::applySkew(Vector2 v)
{
    float x = v.x - v.y * coords.skewY;
    float y = v.y + v.x * coords.skewX;
    return Vector2{x, y};
}

std::tuple<float, float> GraphEditor::getNodeCoordinates(const GraphCoordinates &data, int node, int i, int j)
{
    /*float xc = data.scaleX * data.X[node] - data.scaleY * data.skewY * data.Y[node];
    float yc = data.scaleY * (data.Y[node] + data.scaleX * data.skewX * data.X[node]);
    xc = xc + i * data.scaleX + j * data.skewY;
    yc = yc - j * data.scaleY + i * data.skewX; */
    float xc = data.scaleX * (data.X[node] + i);
    float yc = data.scaleY * (data.Y[node] + j);
    auto transformed = applySkew(Vector2{xc, yc});
    return {transformed.x, transformed.y};
}

GraphEditor::GraphEditor()
{

}

void GraphEditor::setGraph(Graph graph_, GraphCoordinates coords_)
{
    graph = graph_; coords = coords_;
}

std::tuple<Graph, GraphCoordinates> GraphEditor::getGraph()
{
    return {graph, coords};
}

std::tuple<int, int, int> GraphEditor::getStartingNode()
{
    if (markedNode.has_value())
    {
        return {markedNode.value().index, markedNode.value().cellx, markedNode.value().celly};
    }
    else
    {
        return {0,0,0};
    }
}

void GraphEditor::handleKeyEvent(KeyEvent event)
{
    if (event.key == KEY_A)
    {
        TESFLAG = true;
    }
    else if(event.key == KEY_B)
    {
        TESFLAG = false;
    }
    if (event.type == KeyEvent::Type::PRESSED)
    {
        if (event.key == KEY_KP_ADD)
        {
            SCALE = SCALE * 1.1;
        }
        else if (event.key == KEY_KP_SUBTRACT)
        {
            SCALE = SCALE * 0.9;
        }
        else if (event.key == KEY_C)
        {
            graph = Graph();
            coords = GraphCoordinates();
            markedNode = std::nullopt;
        }
        else if (event.key == KEY_H)
        {
            auto [g, c] = getHexGrid();
            graph = g;
            coords = c;
        }
        else if (event.key == KEY_RIGHT)
        {
            coords.skewY += 0.05;
        }
        else if (event.key == KEY_LEFT)
        {
            coords.skewY -= 0.05;
        }
        else if (event.key == KEY_UP)
        {
            coords.skewX += 0.05;
        }
        else if (event.key == KEY_DOWN)
        {
            coords.skewX -= 0.05;
        }
        else if (event.key == KEY_Z)
        {
            if (IsKeyDown(KEY_LEFT_CONTROL))
            {
                if (!history.empty())
                {
                    auto [g, c] = history.top();
                    graph = g;
                    coords = c;
                    history.pop();
                    markedNode = std::nullopt;
                }
            }
            
        }
        
    }
    
}

void GraphEditor::handleMouseEvent(MouseEvent event)
{
    if (event.type == MouseEvent::Type::PRESSED)
    {
        switch (event.button)
        {
        case MOUSE_BUTTON_LEFT:
            handleLeftClick(event);
            break;
        case MOUSE_BUTTON_RIGHT:
            handleRightClick(event);
            break;
        default:
            break;
        }
    }

    if (event.type == MouseEvent::Type::MOVED)
    {
        if (editState == State::PRE_MOVE)
        {
            editState = State::MOVING;
        }
        
    }
    
    
}

void GraphEditor::handleMouseMovements()
{
    if (editState == State::MOVING)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            if (markedNode.has_value())
            {
                auto mouse = GetMousePosition();
                auto cord = screenToCoords(mouse);

                coords.X[markedNode.value().index] = cord.x - markedNode.value().cellx;
                coords.Y[markedNode.value().index] = cord.y - markedNode.value().celly;
            }
        }
        else
        {
            editState = State::DRAWING;
        }
    }
    
}

GraphEditor::~GraphEditor()
{

}

void GraphEditor::draw()
{
    //DrawRectangle(-0.5*SCALE + OFF_X, -0.5*SCALE + OFF_Y, SCALE, SCALE, LIGHTGRAY);
    drawUnitCell();

    for (int i = -5; i < 6; i++)
    {
        for (int j = -4; j < 5; j++)
        {
            drawCellEdges(i, j);
        }
    }

    for (int i = -5; i < 6; i++)
    {
        for (int j = -4; j < 5; j++)
        {
            drawCellNodes(i, j);
        }
    }

    if (markedNode.has_value())
    {
        auto [x, y] = getNodeCoordinates(coords, markedNode->index, markedNode->cellx, markedNode->celly);
        DrawCircle(static_cast<int>(SCALE * x) + OFF_X, static_cast<int>(SCALE * y) + OFF_Y, RADIUS, RED);

        if (editState == State::DRAWING)
        {
            auto start = Vector2{SCALE * x + OFF_X, SCALE * y + OFF_Y};
            auto end = GetMousePosition();
            DrawLineEx(start, end, 4, BLUE);
        }
    }

    handleMouseMovements();
}

