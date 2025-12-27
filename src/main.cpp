#include <iostream>
#include <format>
#include <random>
#include <fstream>
#include <string>
#include <sstream>


#include "Graph.hpp"
#include "MCSimulation.hpp"
#include "PostProcessor.hpp"
#include "Utilities.hpp"

#ifdef USE_GUI
    #include "GUI.hpp"
#endif

std::tuple<Graph, GraphCoordinates> getDefaultSquareGrid()
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

void printNodes(const MCSimulation::Nodes &data)
{
    for (size_t i = 0; i < data.i.size(); i++)
    {
        std::cout << std::format("{:5} {:5}",data.x[i],data.y[i]) << std::endl;
    }
}

void runCLI()
{
    auto cfg = parseConfigFile();

    std::random_device seedgen;

    std::tuple<Graph, GraphCoordinates> result;

    if (!cfg.filename.empty())
    {
        result = loadGraphFromFile(cfg.filename);
    }
    else
    {
        result = getDefaultSquareGrid();
        std::cout << "Running simulation on a square grid" << std::endl;
    }

    auto [graph, coords] = result;

    auto sim = MCSimulation(graph, coords, cfg, seedgen());

    sim.run();

    PostProcessor::writeResultsToFile(sim, coords);
    
}

int main(int argc, char *argv[])
{ 
    #ifdef USE_GUI
        GUI gui;
        gui.start();
    #elif defined(USE_CLI)
        runCLI();
    #else
        #error "Build mode not specified!"
    #endif

    return 0;
}