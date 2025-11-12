#include "Graph.hpp"

Graph::Graph()
{
    data.N = 0; //initializes the number of nodes to 0
}

void Graph::addNode()
{
    data.N++; //increases the node counter
    data.edge_start.push_back(data.vert_A.size()); //adds a new entry to the edge start array, the edges for this node will start at the end of the array
    data.edges_per_node.push_back(0);
}

void Graph::addBidirectionalEdge(int vertA, int vertB, int incX, int incY)
{
    addEdge(vertA,vertB,incX,incY);
    addEdge(vertB,vertA,-incX,-incY);
}

void Graph::addEdge(int vertA, int vertB, int incX, int incY)
{
    if (vertA >= data.N)
    {
        throw std::invalid_argument("Invalid vertex index. Vertex A is greater than the number of vertices");
    }

    //first find the index where we should add it
    int id = data.edge_start[vertA];

    //insert edge
    data.vert_A.insert(data.vert_A.begin()+id, vertA);
    data.vert_B.insert(data.vert_B.begin()+id, vertB);
    data.inc_X .insert(data.inc_X .begin()+id, incX);
    data.inc_Y .insert(data.inc_Y .begin()+id, incY);

    //increment the number of edges on the node
    data.edges_per_node[vertA]++;

    //for the rest of the nodes, the array was shifted, thus we increment the starting position
    for (size_t i = vertA+1; i < data.edge_start.size(); i++)
    {
        data.edge_start[i]++;
    }
}

std::vector<int> Graph::getEdgesFromNode(int node) const
{
    auto to_ret = std::vector<int>();
    for (size_t i = 0; i < data.vert_A.size(); i++)
    {
        if (data.vert_A[i] == node)
        {
            to_ret.push_back(data.vert_B[i]);
            to_ret.push_back(data.inc_X[i]);
            to_ret.push_back(data.inc_Y[i]);
        }
    }

    return to_ret;
}

const GraphData &Graph::getGraphDataConst()
{
    return data;
}

GraphData Graph::getGraphData()
{
    return data;
}

Graph::~Graph()
{
}
