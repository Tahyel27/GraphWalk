#include <iostream>

#include "Graph.hpp"
#include "MCSimulation.hpp"

int main()
{
    Graph graph;
    MCSimulation sim(graph, 10, 100);
    std::cout << "Hello world" << std::endl;
    return 0;
}