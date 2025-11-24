#include <iostream>
#include <format>
#include <random>

#include "Graph.hpp"
#include "MCSimulation.hpp"

void printNodes(const MCSimulation::Nodes &data)
{
    for (size_t i = 0; i < data.i.size(); i++)
    {
        std::cout << std::format("{:5} {:5}",data.x[i],data.y[i]) << std::endl;
    }
}

void printStep(const SimulationData &data, long long step)
{
    auto stepData = data.getStep(step);
    for (size_t i = 0; i < stepData.size(); i++)
    {
        std::cout << std::format("{:5} {:5}", stepData[i].x, stepData[i].y) << std::endl;
    }
}

int main()
{
    Graph graph;
    graph.addNode();
    graph.addBidirectionalEdge(0,0,1,0);
    graph.addBidirectionalEdge(0,0,0,1);

    std::random_device seedgen;

    MCSimulation sim(graph, 1000, 30, seedgen());

    sim.setDataStore(std::make_unique<SimulationData>());

    sim.run();

    auto res = sim.getData();
    auto data = sim.getDataPointer();

    printStep(*data, 4);

    //printNodes(res);

    return 0;
}