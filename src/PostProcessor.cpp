#include "PostProcessor.hpp"


PostProcessor::PostProcessor(MCSimulation &sim_) : sim(sim_)
{

}

double PostProcessor::getLength(double x, double y)
{
    return sqrt(x * x + y * y);
}

void PostProcessor::setRepresentation(GraphCoordinates rep_)
{
    representation = rep_;
}

std::vector<float> PostProcessor::getR_n() const
{
    auto dataptr = sim.getDataPointer();

    auto ret = std::vector<float>(dataptr->getStepCount());

    for (size_t i = 0; i < dataptr->getStepCount(); i++)
    {
        //calculate mean R(n) for n-th step
        auto step = dataptr->getStep(i);

        double lengthCumul = 0;
        for (size_t j = 0; j < step.size(); j++)
        {
            double x = step[j].x + representation.X[step[j].id];
            double y = step[j].y + representation.Y[step[j].id];
            double x_trans = representation.scaleX * ( x + representation.skewY * y);
            double y_trans = representation.scaleY * ( y + representation.skewX * x);
            lengthCumul += getLength(x_trans, y_trans);
        }

        ret[i] = lengthCumul / static_cast<double>(step.size());
    }
    
    return ret;
}

std::vector<float> PostProcessor::getR_n_parallel() const
{
    auto dataptr = sim.getDataPointer();

    auto ret = std::vector<float>(dataptr->getStepCount());

    //each step has to be calculated independently
    for (size_t i = 0; i < dataptr->getStepCount(); i++)
    {
        auto length_cumul = std::vector<double>();

        //#pragma omp parallel firstprivate(representation)
        {
            //#pragma omp single
            {
                length_cumul = std::vector<double>(omp_get_num_threads(),0);
            }

            //#pragma omp for
            for (size_t j = 0; j < dataptr->getRunCount(); j++)
            {
                int th_num = omp_get_thread_num();
                auto step = dataptr->parallelLoad(j, i);
                double x = step.x + representation.X[step.id];
                double y = step.y + representation.Y[step.id];
                double x_trans = representation.scaleX * (x + representation.skewY * y);
                double y_trans = representation.scaleY * (y + representation.skewX * x);
                length_cumul[th_num] += getLength(x_trans, y_trans);
            }
        }

        double lc = 0;
        for (size_t j = 0; j < length_cumul.size(); j++)
        {
            lc += length_cumul[j];
        }
        
        ret[i] = lc / dataptr->getRunCount();

    } 

    return ret;
}

std::vector<long long> PostProcessor::getReturnedToOrigin() const
{
    auto dataptr = sim.getDataPointer();
    //iterate over runs
    auto retStep = std::vector<long long> (dataptr->getRunCount());
    #pragma omp parallel for
    for (size_t i = 0; i < dataptr->getRunCount(); i++)
    {
        for (size_t j = 1; j < dataptr->getStepCount(); j++)
        {
            auto step = dataptr->parallelLoad(i, j);
            if (step.x == 0 && step.y == 0)
            {
                retStep[i] = j;
                break;
            }
        }
    }

    auto ret = std::vector<long long> (dataptr->getStepCount(),0);
    //pull together data from all the runs
    for (size_t i = 0; i < retStep.size(); i++)
    {
        ret[retStep[i]] += 1;
    }

    for (size_t i = 2; i < ret.size(); i++)
    {
        ret[i] += ret[i-1];
    }
    

    return ret;
}

void PostProcessor::writeResultsToFile(MCSimulation &sim, const GraphCoordinates &coords)
{
    auto distanceFile = std::ofstream("distance.dat", std::ios::trunc);
    auto retFile = std::ofstream("return.dat", std::ios::trunc);

    PostProcessor proc(sim);
    proc.setRepresentation(coords);

    auto R_n = proc.getR_n_parallel();
    auto ret = proc.getReturnedToOrigin();

    auto writePeriod = sim.getWritePeriod();
    
    for (size_t i = 0; i < R_n.size(); i++)
    {
        distanceFile << std::format("{:6}  {:6.5f}\n", writePeriod*i, R_n[i]);
    }

    float percent = 100.f* static_cast<float>(ret[0])/static_cast<float>(sim.getDataPointer()->getRunCount());

    retFile << std::format("Did not return to origin: {:10}  ({:3.1f}%)\n", ret[0], percent);
    retFile << std::format("{:6} {:6}", "iter", "n_ret\n");
    for (size_t i = 1; i < ret.size(); i++)
    {
        retFile << std::format("{:6}  {:6}\n", writePeriod * i, ret[i]);
    }
    
    distanceFile.close();
    retFile.close();

    std::cout << "Results written to files\n";
    std::cout << std::format("Did not return to origin: {:10}  ({:3.1f}%)\n", ret[0], percent);
}
