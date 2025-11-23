#include "MCSimulation.hpp"

MCSimulation::MCSimulation(const Graph &graph_, int runs, long long steps, unsigned long seed)
{
    graph = graph_.getGraphData();

    totalSteps = steps;
    totalRuns = runs;
    data.i.resize(runs);
    data.x.resize(runs);
    data.y.resize(runs);

    generator = XoshiroCpp::Xoroshiro128PlusPlus(seed);

    initAtZero();
}

void MCSimulation::initAtZero()
{
    std::fill(data.i.begin(), data.i.end(), 0);
    std::fill(data.x.begin(), data.x.end(), 0);
    std::fill(data.y.begin(), data.y.end(), 0);
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