#include "MCSimulation.hpp"

MCSimulation::MCSimulation(const Graph &graph_, int runs, long long steps)
{
    graph = graph_.getGraphData();

    totalSteps = steps;
    totalRuns = runs;
}

void MCSimulation::run()
{
    //iterate over multiple runs
    for (size_t i = 0; i < totalRuns; i++)
    {
        //perform a run
        for (size_t i = 0; i < totalSteps; i++)
        {
            step(i);
        }
    }    
}

MCSimulation::data_t MCSimulation::getData()
{
    return data.i;
}
