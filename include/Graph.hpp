#pragma once

#include <vector>
#include <stdexcept>
#include <tuple>

struct GraphData
{
    int N = 0; // number of vertices in the graph
    // edges are stored a struct of arrays for optimal performance
    std::vector<int> vert_A; // the starting vertex of an edge
    std::vector<int> vert_B; // the terminating vertex of an edge
    std::vector<int> inc_X;  // the X direction unit cell increment of an edge
    std::vector<int> inc_Y;  // the Y direction unit cell increment of an edge

    // arrays that store information on how the edges are organized
    std::vector<int> edge_start; // the starting index of a set of edges for a given vertex
    std::vector<int> edges_per_node;
};

struct GraphCoordinates
{
    int N = 0;

    std::vector<float> X;
    std::vector<float> Y;
    
    float skewX = 0;
    float skewY = 0;
    float scaleX = 1;
    float scaleY = 1;
};

class Graph
{
private:
    GraphData data;
public:
    Graph(/* args */);

    void addNode();
    void addBidirectionalEdge(int vertA, int vertB, int incX, int incY);
    void addEdge(int vertA, int vertB, int incX, int incY);

    int getN() const { return data.N; };
    int getEdges() const { return data.vert_A.size(); };
    std::vector<int> getEdgesFromNode(int node) const;

    const GraphData & getGraphDataConst();
    GraphData getGraphData();
    GraphData getGraphData() const;

    ~Graph();
};

std::tuple<Graph, GraphCoordinates> getHexGrid();