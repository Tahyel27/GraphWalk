#include <iostream>
#include <format>

#include "Graph.hpp"
#include "MCSimulation.hpp"

void printNodes(const MCSimulation::Nodes &data)
{
    for (size_t i = 0; i < data.i.size(); i++)
    {
        std::cout << std::format("{:5} {:5}",data.x[i],data.y[i]) << std::endl;
    }
}

int main()
{
    Graph graph;
    MCSimulation sim(graph, 10, 100);

    sim.run();

    auto res = sim.getData();

    printNodes(res);

    return 0;
}