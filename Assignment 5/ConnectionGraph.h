//
//  ConnectionGraph.hpp
//  Tutorial 20
//
//  Created by Favour Olaifa-Olawale on 2023-11-08.
//

#ifndef ConnectionGraph_h
#define ConnectionGraph_h
#define GL_SILENCE_DEPRECATION


#include "Vector3.h"
#include "GetGlut.h"
#include "Map.h"
#include "Path.h"

#include <queue>

using namespace std;
using namespace ObjLibrary;

//const unsigned int LINK_TYPE_BASIC     = 0;
//const unsigned int LINK_TYPE_DOWN_ONLY = 1;
const unsigned int LINK_TYPE_DIAGONAL  = 0;
const unsigned int LINK_TYPE_ADJACENT = 1;
const unsigned int LINK_TYPE_UP_1 = 2;
const unsigned int LINK_TYPE_DOWN_1 = 3;
const unsigned int LINK_TYPE_DOWN_FAR = 4;
const unsigned int LINK_TYPE_JUMP2 = 5;
const unsigned int LINK_TYPE_JUMP3 = 6;
const unsigned int LINK_TYPE_COUNT = 7;

const double SEARCH_VALUE_NOT_SET = -1.0;
const unsigned int PREVIOUS_NOT_SET = 999999999;
const unsigned int PREVIOUS_START   = 999999998;



struct Node
{
  Vector3 position;
    vector<Link> links;
};

struct Prioritized
{
  unsigned int node_index;
  double priority;
};


class ConnectionGraph {

public:
    ConnectionGraph();
    ConnectionGraph(const Map& map);
    unsigned int getNodeCount() const;
    unsigned int getNodeIndexAt(int x, int z) const;
    const ObjLibrary::Vector3& getNodePosition(unsigned int node_index) const;
    void drawNodes() const; // was drawConnectionGraphNodes
    void drawLinks() const; // was drawConnectionGraphLinks
    void addNodeLinks(Node& node, int x, int z);
    void addStraightLink(Node& node, double start_y, int end_x, int end_z);// was addOneLink
    void addDiagonalLink(Node& node, double start_y, int start_x, int start_z, int end_x, int end_z);
    void addJump2Link(Node& node, double start_y, int mid_x, int mid_z, int end_x, int end_z);
    void addJump3Link(Node& node, double start_y, int mid1_x, int mid1_z, int mid2_x, int mid2_z, int end_x, int end_z);
    Path breadthFirstSearch (unsigned int start,unsigned int goal);
    Path bestFirstSearch (unsigned int start,unsigned int goal);
    Path aStarSearch (unsigned int start,unsigned int goal);
    Path calculatePath (const std::vector<unsigned int>& previous_nodes,const std::vector<unsigned int>& previous_links,const std::vector<double>& search_values,
                                           unsigned int current);
    void  setColorForLink (unsigned int link_type) const;
    double getTraversalCost (unsigned int link_type) const;

private:
    static const unsigned int GRID_SIZE_X = 38;
    static const unsigned int GRID_SIZE_Z = 20;
    //double HEIGHT_GRID[GRID_SIZE][GRID_SIZE];
    vector<Node> nodes;
    unsigned int node_indexes[GRID_SIZE_X][GRID_SIZE_Z];
    Link toLink (unsigned int destination, unsigned int link_type)
    {
        Link link;
        link.destination = destination;
        link.link_type = link_type;
        return link;
    }
    Prioritized toPrioritized (unsigned int node_index,
                               double priority)
    {
        Prioritized prioritized;
          prioritized.node_index = node_index;
          prioritized.priority   = priority;
          return prioritized;
    }

    
};



bool operator< (const Prioritized& lhs,
                const Prioritized& rhs);

#endif /* ConnectionGraph_hpp */







