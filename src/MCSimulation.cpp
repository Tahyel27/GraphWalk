#include "MCSimulation.hpp"

MCSimulation::MCSimulation(const Graph &graph_, int runs, long long steps)
{
    graph = graph_.getGraphData();

    totalSteps = steps;
    totalRuns = runs;
    data.i.resize(runs);
    data.x.resize(runs);
    data.y.resize(runs);

    initAtZero();
}

void MCSimulation::run()
{
    //iterate over multiple runs
    for (size_t i = 0; i < totalRuns; i++)
    {
        //perform a run
        for (size_t j = 0; j < totalSteps; j++)
        {
            step(i);
        }
    }    
}

MCSimulation::Nodes MCSimulation::getData()
{
    return data;
}