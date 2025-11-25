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
        auto reps = std::vector<GraphCoordinates>();

        #pragma omp parallel
        {
            #pragma omp single
            {
                length_cumul = std::vector<double>(omp_get_num_threads(),0);
                reps = std::vector<GraphCoordinates>(omp_get_num_threads(),representation);
            }

            #pragma omp for
            for (size_t j = 0; j < dataptr->getRunCount(); j++)
            {
                int th_num = omp_get_thread_num();
                auto step = dataptr->parallelLoad(j, i);
                double x = step.x + reps[th_num].X[step.id];
                double y = step.y + reps[th_num].Y[step.id];
                double x_trans = reps[th_num].scaleX * (x + reps[th_num].skewY * y);
                double y_trans = reps[th_num].scaleY * (y + reps[th_num].skewX * x);
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
