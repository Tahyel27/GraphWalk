#include <iostream>
#include <format>
#include <random>

#include "Graph.hpp"
#include "MCSimulation.hpp"
#include "PostProcessor.hpp"

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

void printRn(const PostProcessor &proc)
{
    auto rn = proc.getR_n_parallel();
    int i = 0;
    for (auto &v: rn)
    {
        std::cout << std::format("{:5}: {:5}\n", i, v);
        i++;
    }
}

void printReturnTime(const PostProcessor &proc)
{
    auto rt = proc.getReturnedToOrigin();
    int i = 0;
    for (auto &v : rt)
    {
        std::cout << std::format("{:5}: {:5}\n", i, v);
        i++;
    }
}

int main()
{
    Graph graph;
    graph.addNode();
    graph.addBidirectionalEdge(0,0,1,0);
    graph.addBidirectionalEdge(0,0,0,1);
    //graph.addBidirectionalEdge(0,0,0,10);

    GraphCoordinates coords;
    coords.N = 1;
    coords.scaleX = 1;
    coords.scaleY = 1;
    coords.skewX = 0;
    coords.skewY = 0;
    coords.X = {0};
    coords.Y = {0};

    std::random_device seedgen;

    MCSimulation sim(graph, 100000, 5000, seedgen());

    sim.setDataStore(std::make_unique<SimulationData>());

    sim.run();

    auto res = sim.getData();
    auto data = sim.getDataPointer();

    auto post = PostProcessor(sim);
    post.setRepresentation(coords);

    //printStep(*data, 10);
    printReturnTime(post);

    //printRn(post);
    //printNodes(res);

    return 0;
}