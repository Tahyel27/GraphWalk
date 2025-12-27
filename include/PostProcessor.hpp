#pragma once

#include <cmath>
#include <fstream>
#include <format>
#include <iostream>
#include "MCSimulation.hpp"
#include "Graph.hpp"
#include "omp.h"

class PostProcessor
{
private:
    MCSimulation &sim;

    GraphCoordinates representation;

    static double getLength(double x, double y);
public:
    explicit PostProcessor(MCSimulation &sim_);

    void setRepresentation(GraphCoordinates rep_);

    static void writeResultsToFile(MCSimulation &sim, const GraphCoordinates &coords);

    ~PostProcessor(){};
};
