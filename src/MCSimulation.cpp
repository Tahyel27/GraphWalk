#include "MCSimulation.hpp"

MCSimulation::MCSimulation(const Graph &graph_, int runs, long long steps, unsigned long seed, long long writeFreq)
{
    graph = graph_.getGraphData();

    totalSteps = steps;
    totalRuns = runs;
    data.i.resize(runs);
    data.x.resize(runs);
    data.y.resize(runs);

    generator = XoshiroCpp::Xoroshiro128PlusPlus(seed);
    generators[0] = XoshiroCpp::Xoroshiro128PlusPlus(seed);

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
    if (totalSteps % writePeriod == 0)
    {
        data_store->reserveSpace(totalRuns, (totalSteps) / writePeriod);
    }
    else
    {
        data_store->reserveSpace(totalRuns, (totalSteps) / writePeriod + 1);
    }
}

SimulationData *MCSimulation::getDataPointer()
{
    return data_store.get();
}

void MCSimulation::run()
{
    //iterate over steps
    /*for (size_t i = 0; i < totalSteps; i++)
    {
        #pragma omp parallel
        {
            #pragma omp single
            {
                //set up a generator for each thread, the .jump() jumps 2^64 in the sequence of the random number generator
                int numThreads = omp_get_num_threads();
                for (size_t i = 1; i < numThreads; i++)
                {
                    generators[i] = generators[i-1];
                    generators[i].jump();
                }
            }

        }
        
        #pragma omp parallel for
        for (size_t j = 0; j < totalRuns; j++)
        {
            data_store->parallelStore(j, i, data.i[j], data.x[j], data.y[j]);
            step(j);
        }        
    }*/
    #pragma omp parallel
    {
        #pragma omp single
        {
            int numThreads = omp_get_num_threads();
            for (size_t i = 1; i < numThreads; i++)
            {
                generators[i] = generators[i - 1];
                generators[i].jump();
            }
        }

        #pragma omp for
        for (size_t i = 0; i < totalRuns; i++)
        {
            int th_num = omp_get_thread_num();
            for (size_t j = 0; j < totalSteps; j++)
            {
                if(j % writePeriod == 0)
                {
                    data_store->parallelStore(i, j / writePeriod, data.i[i], data.x[i], data.y[i]);
                }
                step(i, generators[th_num]);
            }
            
        }
        
    }

}

MCSimulation::Nodes MCSimulation::getData()
{
    return data;
}

void MCSimulation::setParams(long long runs, long long steps, long long writePeriod_)
{
    totalRuns = runs;
    totalSteps = steps;
    writePeriod = writePeriod_;

    if (totalSteps % writePeriod == 0)
    {
        data_store->reserveSpace(totalRuns, (totalSteps) / writePeriod);
    }
    else
    {
        data_store->reserveSpace(totalRuns, (totalSteps) / writePeriod + 1);
    }

    data.i.resize(runs);
    data.x.resize(runs);
    data.y.resize(runs);
}

void MCSimulation::setStartingPosition(int index, int cellx, int celly)
{
    std::fill(data.i.begin(), data.i.end(), index);
    std::fill(data.x.begin(), data.x.end(), cellx);
    std::fill(data.y.begin(), data.y.end(), celly);
}

long long MCSimulation::getWritePeriod()
{
    return writePeriod;
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
    return id.size() / runsStored;
}

long long SimulationData::getRunCount()
{
    return runsStored;
}

void loadSimulationFromConfigFile(MCSimulation &sim)
{
    auto configFile = std::ifstream("config.txt");

    long long runs = 0;
    long long steps = 0;
    long long wtperiod = 0;

    std::string line;
    while (std::getline(configFile, line))
    {
        auto name = line.substr(0, line.find(" "));
        if (name == "runs")
        {
            auto num = line.substr(line.find(" ")+1, line.size() - line.find(" "));
            runs = std::stoll(num);
        }
        if (name == "steps")
        {
            auto num = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
            steps = std::stoll(num);
        }
        if (name == "writeperiod")
        {
            auto num = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
            wtperiod = std::stoll(num);
        }
    }

    sim.setParams(runs, steps, wtperiod);
    
}
