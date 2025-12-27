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

void MCSimulation::initGenerators(int threadCount)
{
    for (size_t i = 1; i < threadCount; i++)
    {
        generators[i] = generators[i - 1];
        generators[i].jump();
    }
}

void MCSimulation::run(const size_t startingStep, const size_t steps, SimulationData &chunkData)
{
    #pragma omp parallel
    {
        #pragma omp for
        for (size_t i = 0; i < totalRuns; i++)
        {
            int th_num = omp_get_thread_num();
            for (size_t j = 0; j < steps; j++)
            {    
                chunkData.parallelStore(i, j, data.i[i], data.x[i], data.y[i]);
                step(i, generators[th_num]);
            }
        }
    }
}

void MCSimulation::calculateRnForChunk(const size_t startingStep, const size_t steps, const SimulationData &chunkData)
{
    const float N = static_cast<float>(totalRuns);
    
    for (size_t i = 0; i < steps; i++)
    {
        const size_t idx = i + startingStep;
        float X_cumul = 0.f;
        float Y_cumul = 0.f;
        float R_cumul = 0.f;

        #pragma omp parallel for reduction(+:X_cumul, Y_cumul, R_cumul)
        for (int run = 0; run < totalRuns; run++)
        {
            const auto state = chunkData.parallelLoad(run, i);
            const float xtmp = state.x + coordinates.X[state.id];
            const float ytmp = state.y + coordinates.Y[state.id];
            const float x = coordinates.scaleX * ( xtmp + coordinates.skewY * ytmp );
            const float y = coordinates.scaleY * ( ytmp + coordinates.skewX * xtmp );
            X_cumul += x;
            Y_cumul += y;
            R_cumul += sqrtf(x * x + y * y);
        }

        const float X_mean = X_cumul / N;
        const float Y_mean = Y_cumul / N;
        const float R_mean = R_cumul / N;

        float Xstd_cumul = 0.f;
        float Ystd_cumul = 0.f;
        float Rstd_cumul = 0.f;

        #pragma omp parallel for reduction(+:Xstd_cumul, Ystd_cumul, Rstd_cumul)
        for (size_t run = 0; run < totalRuns; run++)
        {
            const auto state = chunkData.parallelLoad(run, i);
            const float xtmp = state.x + coordinates.X[state.id];
            const float ytmp = state.y + coordinates.Y[state.id];
            const float x = coordinates.scaleX * (xtmp + coordinates.skewY * ytmp);
            const float y = coordinates.scaleY * (xtmp + coordinates.skewX * xtmp);
            Xstd_cumul += (x - X_mean) * (x - X_mean);
            Ystd_cumul += (y - Y_mean) * (y - Y_mean);
            const float R = sqrtf(x * x + y * y);
            Rstd_cumul += (R - R_mean) * (R - R_mean);
        }

        results.R[idx] = R_mean;
        results.X[idx] = X_mean;
        results.Y[idx] = Y_mean;
        results.sigma_R[idx] = Rstd_cumul;
    }
}

void MCSimulation::calculateReturnsForChunk(const size_t startingStep, const size_t steps, const SimulationData &chunkData, std::vector<size_t> &returnTimes, long long sI, long long sX, long long sY)
{
    #pragma omp parallel for
    for (size_t run = 0; run < totalRuns; run++)
    {
        if (returnTimes[run] == 0)
        {
            for (size_t step = 0; step < steps; step++)
            {
                auto state = chunkData.parallelLoad(run, step);
                if (state.id == sI && state.x == sX && state.y == sY)
                {
                    returnTimes[run] = startingStep + step;
                    if (startingStep + step != 0)
                    {
                        break;
                    }
                    
                }
            }
        }
    }    
}

void MCSimulation::calculateReturnTimes(const std::vector<size_t> &returnTracker)
{
    auto tmp = std::vector<size_t>(totalSteps,0);
    
    for (size_t run = 0; run < totalRuns; run++)
    {
        tmp[returnTracker[run]]++;
    }

    size_t cumul = 0;
    for (size_t step = 1; step < totalSteps; step++)
    {
        cumul += tmp[step];
        results.returns[step] = cumul;
    }
    results.returns[0] = totalRuns - cumul;
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

void MCSimulation::runChunked()
{
    const size_t max_memory = 7ULL*1024ULL*1024ULL*1024ULL; //6GiB
    const size_t chunksize = std::min(totalSteps,max_memory/(sizeof(long long)*3*totalRuns));
    const int thread_count = omp_get_max_threads();

    omp_set_num_threads(thread_count);

    initGenerators(thread_count);

    results.resize(totalSteps);
    
    auto data = SimulationData{};
    data.reserveSpace(totalRuns, chunksize);

    auto retTracker = std::vector<size_t>(totalRuns, 0);

    const size_t chunk_count = totalSteps / chunksize;

    for (size_t i = 0; i < chunk_count; i++)
    {
        run(i*chunksize, chunksize, data);

        calculateRnForChunk(i*chunksize, chunksize, data);

        calculateReturnsForChunk(i*chunksize, chunksize, data, retTracker, 0, 0, 0);
    }
    
    if (totalSteps - chunk_count * chunksize > 0)
    {
        run(chunk_count * chunksize, totalSteps - chunk_count * chunksize, data);

        calculateRnForChunk(chunk_count * chunksize, totalSteps - chunk_count * chunksize, data);
    }

    calculateReturnTimes(retTracker);
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

    /*
    if (totalSteps % writePeriod == 0)
    {
        data_store->reserveSpace(totalRuns, (totalSteps) / writePeriod);
    }
    else
    {
        data_store->reserveSpace(totalRuns, (totalSteps) / writePeriod + 1);
    }*/

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

SimulationResults MCSimulation::getResults() const
{
    return results;
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
