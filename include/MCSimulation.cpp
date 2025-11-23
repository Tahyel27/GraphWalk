#include "MCSimulation.hpp"

MCSimulation::MCSimulation(const Graph &graph_, int runs, long long steps)
{
    graph = graph_.getGraphData();

    totalSteps = steps;
    totalRuns = runs;
}

MCSimulation::data_t MCSimulation::getData()
{
    return data.i;
}
