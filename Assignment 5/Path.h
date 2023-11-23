//
//  Path.hpp
//  Tutorial 21
//
//  Created by Favour Olaifa-Olawale on 2023-11-13.
//

#ifndef Path_h
#define Path_h

#include <stdio.h>
#include <vector>
#include <queue>
#include "Vector3.h"

class ConnectionGraph;

struct Link
{
  unsigned int destination;
  unsigned int link_type;
  // link type will go here
};

class Path {
public:
  
    Path();
    Path(unsigned int start_index_in, const std::vector<double>& search_values_in);
    bool isEmpty() const;
    void resetLinkTimer();
    void updateLinkTimer(double delta_time);
    double getLinkTimer()const ;
    void   drawSearchValues (ConnectionGraph& graph) const;
    const ObjLibrary::Vector3& getStartPosition(const ConnectionGraph& graph) const;
    void drawPath(const ConnectionGraph& graph) const;
    void appendLink(const Link& link);
    const ObjLibrary::Vector3& getSecondNodePosition(const ConnectionGraph& graph) const;
    unsigned int getLinkTypeToSecondNode() const;
    void removeFirstLinkAndSetNewStartNode();
    void FrontToAdjacent();
    void destroyPath();
    
private:
    unsigned int start_index;
    //std::vector<Link> links;
    std::queue<Link> links;
    std::vector<double> search_values;
    double link_timer;
};




#endif /* Path_hpp */
