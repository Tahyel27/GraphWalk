#pragma once

#include <iostream>
#include <format>
#include <random>
#include <fstream>
#include <string>
#include <sstream>

#include "MCSimulation.hpp"
#include "Graph.hpp"

std::tuple<Graph, GraphCoordinates> loadGraphFromFile(const std::string &fname);

void loadSimulationFromConfigFile(MCSimulation &sim);

SimulationConfig parseConfigFile();