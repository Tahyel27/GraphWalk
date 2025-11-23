#pragma once

#include "Graph.hpp"
#include "XoshiroCpp.hpp"
#include <vector>

class MCSimulation
{
private:
    Graph graph;

    XoshiroCpp::Xoroshiro128PlusPlus generator;

public:

    MCSimulation(Graph graph_, int runs, int steps);
    ~MCSimulation();

    void run();
};
