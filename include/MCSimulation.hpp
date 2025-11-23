#pragma once

#include "Graph.hpp"
#include "XoshiroCpp.hpp"
#include <vector>
#include <random>

class MCSimulation
{
public:
    using data_t = std::vector<long long>;

    struct Nodes
    {
        data_t i;
        data_t x;
        data_t y;
    };

    MCSimulation(const Graph &graph_, int runs, long long steps);
    ~MCSimulation(){};

    void run();

    Nodes getData();

private:

    Nodes data;
    
    GraphData graph;

    XoshiroCpp::Xoroshiro128PlusPlus generator;

    long long totalSteps; // number of Monte Carlo steps to take
    long long totalRuns;

    void initAtZero();

    inline void step(int i);
};

inline void MCSimulation::initAtZero()
{
    std::fill(data.i.begin(),data.i.end(), 0);
    std::fill(data.x.begin(),data.x.end(), 0);
    std::fill(data.y.begin(), data.y.end(), 0);
}

inline void MCSimulation::step(int i)
{
    // update for a single run
    int n_edges = graph.edges_per_node[data.i[i]]; // get the number of edges for our location on the periodic graph
    int edge_start = graph.edge_start[data.i[i]];  // get the location of the information about our node in the graph data rep.

    std::uniform_int_distribution<> dist(0, n_edges - 1);

    int choice = dist(generator); // pick an edge

    // now update the position based on the graph data, which is at some_data[edge_start + choice]
    data.i[i] = graph.vert_B[edge_start + choice];
    data.x[i] += graph.inc_X[edge_start + choice];
    data.y[i] += graph.inc_Y[edge_start + choice];
}