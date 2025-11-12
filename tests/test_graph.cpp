#include "test_helpers.hpp"
#include "Graph.hpp"
#include <iostream>
#include <vector>

int testAdditionOfNodesAndTheirCount(){
    Graph g;
    g.addNode();
    g.addNode();
    g.addNode();
    ASSERT(g.getN() == 3, g.getN());
    return 0;
}

int testEmptyEdgeCound(){
    Graph g;
    g.addNode();
    g.addNode();
    g.addNode();
    ASSERT(g.getEdges() == 0, g.getEdges());
    return 0;
}

int testOneBidirectionalEdge(){
    Graph g;
    g.addNode();
    g.addNode();
    g.addBidirectionalEdge(0, 1, 0, 1);
    auto edges = g.getEdgesFromNode(1);
    auto target = std::vector<int>{0,0,-1};
    ASSERT(vector_compare(edges,target), " vectors not same ");
    /*edges = g.getEdgesFromNode(0);
    target = std::vector<int>{1,0,1};
    ASSERT(vector_compare(edges, target), " vectors not same ");*/
    return 0;
}

int testRectangularGrid(){
    Graph g;
    g.addNode();
    g.addBidirectionalEdge(0,0,1,0);
    g.addBidirectionalEdge(0,0,0,1);
    auto edges = g.getEdgesFromNode(0);
    ASSERT(edges.size() == 4*3, edges.size());
    ASSERT(edges[edges.size() - 2] == 1, edges[edges.size() - 2]);
    ASSERT(edges[edges.size() - 2 - 3] == -1, edges[edges.size() - 2 - 3]);
    return 0;
}

int testHexGrid(){
    Graph g;
    g.addNode();
    g.addNode();
    g.addBidirectionalEdge(0,1,0,0);
    g.addBidirectionalEdge(0,1,-1,-1);
    g.addBidirectionalEdge(0,1,0,1);
    auto edges = g.getEdgesFromNode(1);
    ASSERT(edges.size() == 3*3, edges.size());
    return 0;
}

int main()
{
    int f_tests = 0;
    int t_tests = 0;
    
    std::cout << "Testing graph module: " << std::endl;

    RUN_TEST(testAdditionOfNodesAndTheirCount);
    RUN_TEST(testEmptyEdgeCound);
    RUN_TEST(testOneBidirectionalEdge);
    RUN_TEST(testRectangularGrid);
    RUN_TEST(testHexGrid);

    std::cout << "Tests passed " << t_tests - f_tests << "/" << t_tests << std::endl;
    return f_tests;
}