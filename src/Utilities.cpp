#include "Utilities.hpp"

std::tuple<Graph, GraphCoordinates> loadGraphFromFile(const std::string &fname)
{
    auto ifs = std::ifstream(fname);
    if (!ifs.is_open())
    {
        std::cerr << "Error opening file: " << fname << std::endl;
        return {};
    }

    int n;
    float posX = 0;
    float posY = 0;
    int nodeA = 0;
    int nodeB = 0;
    int incX = 0;
    int incY = 0;

    float scaleX;
    float scaleY;
    float skewX;
    float skewY;

    int nodecounter = -1;

    Graph graph;
    GraphCoordinates coords;

    auto line = std::string();
    while (std::getline(ifs, line))
    {
        auto sstream = std::istringstream(line);
        if (line[0] == 's')
        {
            char dummy;
            sstream >> dummy >> scaleX >> scaleY >> skewX >> skewY;
            coords.scaleX = scaleX;
            coords.scaleY = scaleY;
            coords.skewX = skewX;
            coords.skewY = skewY;
        }
        else if (line[0] != '#')
        {
            if (nodecounter == -1)
            {
                sstream >> n;
                nodecounter = n;
                coords.N = n;
            }
            else if (nodecounter > 0)
            {
                sstream >> posX >> posY;
                graph.addNode();
                coords.X.push_back(posX);
                coords.Y.push_back(posY);
                nodecounter--;
            }
            else
            {
                sstream >> nodeA >> nodeB >> incX >> incY;
                graph.addBidirectionalEdge(nodeA, nodeB, incX, incY);
            }
        }
    }
    return std::tuple<Graph, GraphCoordinates>(graph, coords);
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
            auto num = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
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

    sim.setParams(runs, steps);
}

SimulationConfig parseConfigFile()
{
    auto configFile = std::ifstream("config.txt");

    auto config = SimulationConfig{};

    std::string line;
    while (std::getline(configFile, line))
    {
        auto name = line.substr(0, line.find(" "));
        if (name == "runs")
        {
            auto num = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
            config.runs = std::stoll(num);
        }
        if (name == "steps")
        {
            auto num = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
            config.steps = std::stoll(num);
        }
        if (name == "graphfile")
        {
            config.filename = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
        }
        if (name == "maxmem")
        {
            auto num = line.substr(line.find(" ") + 1, line.size() - line.find(" "));
            config.max_memory = std::stoull(num) * 1024ULL * 1024ULL * 1024ULL;
        }
    }

    return config;
}
