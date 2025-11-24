#pragma once

#include <cmath>
#include "MCSimulation.hpp"
#include "Graph.hpp"

class PostProcessor
{
private:
    MCSimulation &sim;

    GraphCoordinates representation;

    static double getLength(double x, double y);
public:
    explicit PostProcessor(MCSimulation &sim_);

    void setRepresentation(GraphCoordinates rep_);

    std::vector<float> getR_n() const;

    ~PostProcessor(){};
};
