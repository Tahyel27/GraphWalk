#include <iostream>
#include <format>
#include <random>
#include <fstream>
#include <string>
#include <sstream>


#include "Graph.hpp"
#include "MCSimulation.hpp"
#include "PostProcessor.hpp"
#include "GUI.hpp"

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

int main(int argc, char *argv[])
{
    Graph graph;
    GraphCoordinates coords;
    
    int numRuns = 1000;
    int numSteps = 1000;
    int writeFreq = 10;
    if (argc == 2)
    {
        numSteps = atoi(argv[0]);
        numRuns = atoi(argv[1]);
    }
    if (argc == 3)
    {
        std::string fname = argv[2];
        std::tie(graph, coords) = loadGraphFromFile(fname);
    }
    else
    {
        std::tie(graph, coords) = getDefaultSquareGrid();
    }
    

    GUI gui;
    gui.start();

    return 0;
}