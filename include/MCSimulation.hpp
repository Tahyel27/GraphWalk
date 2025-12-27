#pragma once

#include "Graph.hpp"
#include "XoshiroCpp.hpp"
#include <vector>
#include <random>
#include <memory>
#include <omp.h>
#include <fstream>
#include <string>

struct SimulationResults
{
    std::vector<int> returns;
    std::vector<float> R;
    std::vector<float> sigma_R;
    std::vector<float> X;
    std::vector<float> Y;

    void resize(const size_t steps)
    {
        returns.resize(steps);
        R.resize(steps);
        sigma_R.resize(steps);
        X.resize(steps);
        Y.resize(steps);
    }
};

class SimulationData
{
public:
    SimulationData(){};
    ~SimulationData(){};

    void reserveSpace(long long runs, long long steps);

    void storeId(const std::vector<long long> &id_, const std::vector<long long> &x, const std::vector<long long> &y);

    inline void parallelStore(long long run, long long step, long long i, long long x, long long y);

    struct Position
    {
        long long id;
        long long x;
        long long y;
    };

    inline Position parallelLoad(long long run, long long step);

    inline Position parallelLoad(long long run, long long step) const;

    std::vector<Position> getStep(long long step) const;

    long long getStepCount();

    long long getRunCount();
private:
    std::vector<long long> id;
    std::vector<long long> X;
    std::vector<long long> Y;

    long long stepsStored = 0;
    long long runsStored  = 0;
};

inline void SimulationData::parallelStore(long long run, long long step, long long i, long long x, long long y)
{
    id[step * runsStored + run] = i;
    X[step * runsStored + run] = x;
    Y[step * runsStored + run] = y;
}

inline SimulationData::Position SimulationData::parallelLoad(long long run, long long step)
{
    return Position{id[step * runsStored + run], X[step * runsStored + run], Y[step * runsStored + run]};
}

inline SimulationData::Position SimulationData::parallelLoad(long long run, long long step) const
{
    return Position{id[step * runsStored + run], X[step * runsStored + run], Y[step * runsStored + run]};
}

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

    MCSimulation(const Graph &graph_, int runs, long long steps, unsigned long seed, long long writeFreq);
    MCSimulation(const Graph &g, const GraphCoordinates &co, size_t runs, size_t steps, unsigned long long seed) :
        graph(g.getGraphData()), coordinates(co), totalRuns(runs), totalSteps(steps) {
            generators[0] = XoshiroCpp::Xoroshiro128PlusPlus(seed);
    };

    ~MCSimulation(){};

    void setDataStore(std::unique_ptr<SimulationData> dataPtr);

    SimulationData * getDataPointer();

    void run();

    void runChunked();

    Nodes getData();

    void setParams(long long runs, long long steps, long long writePeriod_);

    void setStartingPosition(int index, int cellx, int celly);

    long long getWritePeriod();

    long long getRunCount() {return totalRuns;};

    SimulationResults getResults() const;

private:

    std::unique_ptr<SimulationData> data_store;

    Nodes data;
    
    GraphData graph;

    GraphCoordinates coordinates;

    SimulationResults results;

    XoshiroCpp::Xoroshiro128PlusPlus generator;
    std::array<XoshiroCpp::Xoroshiro128PlusPlus, 36> generators;

    size_t totalSteps; // number of Monte Carlo steps to take
    size_t totalRuns;
    long long writePeriod = 10;

    void initAtZero();

    void initGenerators(int threadCount);

    void run(const size_t startingStep, const size_t steps, SimulationData &chunkData);

    void calculateRnForChunk(const size_t startingStep, const size_t steps, const SimulationData &chunkData);

    void calculateReturnsForChunk(const size_t startingStep, const size_t steps, const SimulationData &chunkData, std::vector<size_t> &returnTime , long long sI, long long sX, long long sY);

    void calculateReturnTimes(const std::vector<size_t> &returnTracker);

    inline void step(int i);

    inline void step(int run, XoshiroCpp::Xoroshiro128PlusPlus &gen);
};

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

inline void MCSimulation::step(int run, XoshiroCpp::Xoroshiro128PlusPlus &gen)
{
    // update for a single run
    int n_edges = graph.edges_per_node[data.i[run]]; // get the number of edges for our location on the periodic graph
    int edge_start = graph.edge_start[data.i[run]];  // get the location of the information about our node in the graph data rep.run
    std::uniform_int_distribution<> dist(0, n_edges - 1);

    int choice = dist(gen); // pick an edge

    // now update the position based on the graph data, which is at some_data[edge_start + choice]
    data.i[run] = graph.vert_B[edge_start + choice];
    data.x[run] += graph.inc_X[edge_start + choice];
    data.y[run] += graph.inc_Y[edge_start + choice];
}

void loadSimulationFromConfigFile(MCSimulation &sim);