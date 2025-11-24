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
