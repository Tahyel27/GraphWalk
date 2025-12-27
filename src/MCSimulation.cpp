#include "MCSimulation.hpp"

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
        results.sigma_R[idx] = Rstd_cumul / N;
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
                const auto state = chunkData.parallelLoad(run, step);
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

void MCSimulation::run()
{
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

void MCSimulation::setParams(long long runs, long long steps)
{
    totalRuns = runs;
    totalSteps = steps;

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


long long SimulationData::getStepCount()
{
    return id.size() / runsStored;
}

long long SimulationData::getRunCount()
{
    return runsStored;
}