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

void MCSimulation::setDataStore(std::unique_ptr<SimulationData> dataPtr)
{
    data_store = std::move(dataPtr);
    data_store->reserveSpace(totalRuns, totalSteps);
}

SimulationData *MCSimulation::getDataPointer()
{
    return data_store.get();
}

void MCSimulation::run()
{
    //iterate over steps
    for (size_t i = 0; i < totalSteps; i++)
    {
        /*if (data_store.get() != nullptr)
        {
            data_store->storeId(data.i, data.x, data.y);
        }*/
        #pragma omp parallel for
        for (size_t j = 0; j < totalRuns; j++)
        {
            data_store->parallelStore(j, i, data.i[j], data.x[j], data.y[j]);
            step(j);
        }        
    }

}

MCSimulation::Nodes MCSimulation::getData()
{
    return data;
}

void SimulationData::reserveSpace(long long runs, long long steps)
{
    id.resize(runs * steps);
    X.resize (runs * steps);
    Y.resize (runs * steps);
    runsStored = runs;
}

void SimulationData::storeId(const std::vector<long long> &id_, const std::vector<long long> &x, const std::vector<long long> &y)
{
    std::copy(x.begin(),x.end(),X.begin() + stepsStored * runsStored);
    std::copy(y.begin(),y.end(),Y.begin() + stepsStored * runsStored);
    std::copy(id_.begin(), id_.end(), id.begin() + stepsStored * runsStored);
    stepsStored++;
}

std::vector<SimulationData::Position> SimulationData::getStep(long long step) const
{
    auto toRet = std::vector<Position>(runsStored);
    for (size_t i = 0; i < runsStored; i++)
    {
        toRet[i].id = id[runsStored * step + i];
        toRet[i].x = X[runsStored * step + i];
        toRet[i].y = Y[runsStored * step + i];
    }
    return toRet;
}

long long SimulationData::getStepCount()
{
    return stepsStored;
}
