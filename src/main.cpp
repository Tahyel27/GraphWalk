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
    graph.addNode();
    graph.addBidirectionalEdge(0,0,1,0);
    graph.addBidirectionalEdge(0,0,0,1);

    MCSimulation sim(graph, 10000, 3, 5);

    sim.run();

    auto res = sim.getData();

    printNodes(res);

    return 0;
}