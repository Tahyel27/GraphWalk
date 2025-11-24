#pragma once

#include <vector>
#include <stdexcept>

struct GraphData
{
    int N; // number of vertices in the graph
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
    int N;

    std::vector<float> X;
    std::vector<float> Y;
    
    float skewX;
    float skewY;
    float scaleX;
    float scaleY;
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
