#include "PostProcessor.hpp"


PostProcessor::PostProcessor(MCSimulation &sim_) : sim(sim_)
{

}

void PostProcessor::setRepresentation(GraphCoordinates rep_)
{
    representation = rep_;
}

void PostProcessor::writeResultsToFile(MCSimulation &sim, const GraphCoordinates &coords)
{
    auto distanceFile = std::ofstream("distance.dat", std::ios::trunc);
    auto retFile = std::ofstream("return.dat", std::ios::trunc);

    const auto results = sim.getResults();

    for (size_t i = 0; i < results.R.size(); i++)
    {
        distanceFile << std::format("{:6}  {:6.5f}  {:6.5f}  {:6.5f}\n", i, results.R[i], results.X[i], results.Y[i]);
    }

    float percent = 100.f * static_cast<float>(results.returns[0]) / static_cast<float>(sim.getRunCount());

    retFile << std::format("Did not return to origin: {:10}  ({:3.1f}%)\n", results.returns[0], percent);
    retFile << std::format("{:6} {:6}", "iter", "n_ret\n");
    for (size_t i = 1; i < results.returns.size(); i++)
    {
        retFile << std::format("{:6}  {:6}\n", i, results.returns[i]);
    }

    distanceFile.close();
    retFile.close();

    std::cout << "Results written to files\n";
    std::cout << std::format("Did not return to origin: {:10}  ({:3.1f}%)\n", results.returns[0], percent);
}
