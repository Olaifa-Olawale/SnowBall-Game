//
//  ConnectionGraph.cpp
//  Tutorial 20
//
//  Created by Favour Olaifa-Olawale on 2023-11-08.
//


#include "ConnectionGraph.h"


ConnectionGraph::ConnectionGraph() {

            Node node;
               node.position = Vector3(0.0, 0.0, 0.0);
               nodes.push_back(node);
}

ConnectionGraph::ConnectionGraph(const Map& map) {
    int mapX = map.getSizeX();
    int mapZ = map.getSizeZ();
    //cout<<map.getSizeX();
    for (int x = 0; x < mapX; x++) {
            for (int z = 0; z < mapZ; z++) {
               
                double centerX = static_cast<double>(x) + 0.5;
                double centerZ = static_cast<double>(z) + 0.5;
                
                //HEIGHT_GRID[x][z] =map.getBlockHeight(x,z);
                node_indexes[x][z] = nodes.size();
                Node node;
                node.position = Vector3(centerX, map.getBlockHeight(x,z), centerZ);
                nodes.push_back(node);
                    
            }
        }
    // Create links here
    for (int x = 0; x < mapX; x++) {
        for (int z = 0; z < mapZ; z++) {
            addNodeLinks(nodes[node_indexes[x][z]], x, z);
        }
    }
}




unsigned int ConnectionGraph::getNodeCount() const {
    return nodes.size();
}

unsigned int ConnectionGraph::getNodeIndexAt(int x, int z) const {
    return node_indexes[x][z];
}

const ObjLibrary::Vector3& ConnectionGraph::getNodePosition(unsigned int node_index) const {
    return nodes[node_index].position;
}

void ConnectionGraph::drawNodes() const {
    glColor3d(0.0, 1.0, 1.0);
    glPointSize(10.0);
    glBegin(GL_POINTS);
    for (unsigned int i = 0; i < nodes.size(); i++) {
        Vector3 position = nodes[i].position;
        glVertex3d(position.x, position.y + 0.2, position.z);
    }
    glEnd();
}

void ConnectionGraph::drawLinks() const
{
    glLineWidth(2.0);
    glBegin(GL_LINES);
    
          for(unsigned int i = 0; i < nodes.size(); i++)
          {
               Vector3 start = nodes[i].position;
               // more will be added here
              for(unsigned int j = 0; j < nodes[i].links.size(); j++)
              {
                  const Link& link = nodes[i].links[j];
                  unsigned int end_index = link.destination;
                  Vector3 end = nodes[end_index].position;
                  
                  // drawing will be added here
                if (i < end_index || link.link_type == LINK_TYPE_DOWN_FAR)
                  {
                      if (link.link_type == LINK_TYPE_DOWN_FAR)
                         glColor3d(1.0, 0.0, 1.0);  // Magenta for DownOnly
                     else if (link.link_type == LINK_TYPE_DIAGONAL)
                         glColor3d(0.0, 0.5, 1.0);  // Blue for Diagonal
                     else if (link.link_type == LINK_TYPE_UP_1 || link.link_type == LINK_TYPE_DOWN_1)
                         glColor3d(0.0, 1.0, 0.0);  // Green for Up1 and Down1
                     else if (link.link_type == LINK_TYPE_JUMP2)
                         glColor3d(1.0, 0.5, 0.0);  // Orange for Jump2
                     else if (link.link_type == LINK_TYPE_JUMP3)
                         glColor3d(1.0, 0.0, 0.0);  // Red for Jump3
                     else if (link.link_type == LINK_TYPE_ADJACENT)
                         glColor3d(1.0, 1.0, 0.0); // Yellow for Adjacent
                     else
                         glColor3d(0.0, 0.0, 1.0);  // Default to Blue

                     glVertex3d(start.x, start.y + 0.2, start.z);
                     glVertex3d(end.x, end.y + 0.2, end.z);
                 }
              }
          }
     glEnd();
    glLineWidth(1.0);
}



//Link ConnectionGraph::toLink(unsigned int destination, unsigned int link_type) {
//    Link link;
//    link.destination = destination;
//    link.link_type = link_type;
//    return link;
//}


void ConnectionGraph::addNodeLinks (Node& node, int x, int z)
{
    double start_y = node.position.y;
    if(x >= 1)
        addStraightLink(node, start_y, x - 1, z);
    if(x < GRID_SIZE_X - 1)
        addStraightLink(node, start_y, x + 1, z);
     if(z >= 1)
         addStraightLink(node, start_y, x, z - 1);
     if(z < GRID_SIZE_Z - 1)
         addStraightLink(node, start_y, x, z + 1);
    
    if(x >= 1 && z >= 1)
           addDiagonalLink(node, start_y, x, z, x - 1, z - 1);
    if(x >= 1 && z < GRID_SIZE_Z - 1)
          addDiagonalLink(node, start_y, x, z, x - 1, z + 1);
     if(x < GRID_SIZE_X - 1 && z >= 1)
          addDiagonalLink(node, start_y, x, z, x + 1, z - 1);
     if(x < GRID_SIZE_X - 1 && z < GRID_SIZE_Z - 1)
          addDiagonalLink(node, start_y, x, z, x + 1, z + 1);
    
    //  Jump2 links
       if (x >= 2)
           addJump2Link(node, start_y,  x-1, z,x - 2, z);
       if (x < GRID_SIZE_X - 2)
           addJump2Link(node, start_y,  x+1, z,x + 2, z);
       if (z >= 2)
           addJump2Link(node, start_y, x, z-1, x, z - 2);
       if (z < GRID_SIZE_Z - 2)
           addJump2Link(node, start_y, x, z+1, x, z + 2);
    
    //  Jump3 links
       if (x >= 3)
          // addJump3Link(node, start_y, x-1, z, x - 3, z);
           addJump3Link(node, start_y, x-1, z,x-2,z, x - 3, z);
       if (x < GRID_SIZE_X - 3)
           addJump3Link(node, start_y, x+1, z,x+2,z, x + 3, z);
       if (z >= 3)
           addJump3Link(node, start_y, x, z-1, x,z-2,x, z - 3);
       if (z < GRID_SIZE_Z - 3)
           addJump3Link(node, start_y, x, z+1,x,z+2, x, z + 3);
}


void ConnectionGraph::addStraightLink (Node& node, double start_y,int end_x, int end_z)
{
  unsigned int end_index = node_indexes[end_x][end_z];
  double end_y = nodes[end_index].position.y;
    
        
      if (end_y == start_y) {
          node.links.push_back(toLink(end_index, LINK_TYPE_ADJACENT));
      }
      else if (end_y == start_y + 1.0) {
          node.links.push_back(toLink(end_index, LINK_TYPE_UP_1));
      }
      else if (end_y == start_y - 1.0) {
          node.links.push_back(toLink(end_index, LINK_TYPE_DOWN_1));
      }
      else if (end_y <= start_y - 2.0) {
          node.links.push_back(toLink(end_index, LINK_TYPE_DOWN_FAR));
      }
//      else if(end_y <= start_y - 1.0){
//          node.links.push_back(toLink(end_index,  LINK_TYPE_BASIC));
//      }
    
}

void ConnectionGraph::addDiagonalLink (Node& node, double start_y,
                      int start_x, int start_z,
                      int end_x,   int end_z)
{
    unsigned int end_index  = node_indexes[  end_x][  end_z];
     unsigned int mid1_index = node_indexes[start_x][  end_z];
     unsigned int mid2_index = node_indexes[  end_x][start_z];
     double  end_y = nodes[ end_index].position.y;
     double mid1_y = nodes[mid1_index].position.y;
     double mid2_y = nodes[mid2_index].position.y;
    if( end_y >= start_y - 1.0 &&
        end_y <  start_y + 1.0 &&
        mid1_y <= start_y       &&
        mid2_y <= start_y       &&
        mid1_y <=   end_y       &&
        mid2_y <=   end_y)
     {
          node.links.push_back(toLink(end_index, LINK_TYPE_DIAGONAL));
     }
}


void ConnectionGraph::addJump2Link(Node& node, double start_y, int mid_x, int mid_z, int end_x, int end_z)
{
    unsigned int mid_index = node_indexes[mid_x][mid_z];
    unsigned int end_index = node_indexes[end_x][end_z];

    double mid_y = nodes[mid_index].position.y;
    double end_y = nodes[end_index].position.y;

    
    if (mid_y < start_y && end_y == start_y) {
            node.links.push_back(toLink(end_index, LINK_TYPE_JUMP2));
       }
}

//void ConnectionGraph::addJump3Link(Node& node, double start_y, int mid_x, int mid_z, int end_x, int end_z)
//{
//    unsigned int mid_index = node_indexes[mid_x][mid_z];
//    unsigned int end_index = node_indexes[end_x][end_z];
//
//    double mid_y = nodes[mid_index].position.y;
//    double end_y = nodes[end_index].position.y;
//
//  
//    if (mid_y < start_y && end_y == start_y ) {
//            node.links.push_back(toLink(end_index, LINK_TYPE_JUMP3));
//        }
//    
//}
void ConnectionGraph::addJump3Link(Node& node, double start_y, int mid1_x, int mid1_z, int mid2_x, int mid2_z, int end_x, int end_z)
{
    unsigned int mid1_index = node_indexes[mid1_x][mid1_z];
    unsigned int mid2_index = node_indexes[mid2_x][mid2_z];
    unsigned int end_index = node_indexes[end_x][end_z];

    double mid1_y = nodes[mid1_index].position.y;
    double mid2_y = nodes[mid2_index].position.y;
    double end_y = nodes[end_index].position.y;

    if (mid1_y < start_y && mid2_y < start_y && end_y == start_y) {
        node.links.push_back(toLink(end_index, LINK_TYPE_JUMP3));
        
    }
}

Path ConnectionGraph :: breadthFirstSearch (unsigned int start,
                                            unsigned int goal)
{
  // initialize a vector to hold N copies of a value
    vector<double> local_search_values(nodes.size(), SEARCH_VALUE_NOT_SET);
    vector<unsigned int> previous_nodes(nodes.size(), PREVIOUS_NOT_SET);
    vector<unsigned int> previous_links(nodes.size(), 0);
    
  // add starting node here
    queue<unsigned int> open;
      open.push(start);
    previous_nodes[start] = PREVIOUS_START;
    unsigned int node_counter = 0;
   
  // process nodes here
    while(!open.empty())
     {
          // get a node ftom the open list
          unsigned int current = open.front();
          open.pop();
         
          // set display information here
         local_search_values[current] = node_counter;
                node_counter++;
         
          // check for goal reached here
                if(current == goal)
                {
                     // calculate the path here
                    // calculate the path
                               //Path path;
                               //path.start_index = start;
                                //49
                               //path.nodes.push_back(goal);
                     return calculatePath(previous_nodes, previous_links, local_search_values,current);
                }
         
          // add nodes to queue here
         for(unsigned int i = 0; i < nodes[current].links.size(); i++)
               {
                    unsigned int destination = nodes[current].links[i].destination;
                   if(previous_nodes[destination] == PREVIOUS_NOT_SET)
                               {
                                    open.push(destination);
                                    previous_nodes[destination] = current;
                                   previous_links[destination] = i;
                               }
               }
     }  // end of while loop
    // no path exists
    Path path(start,local_search_values);
//    path.start_index = start;
//    path.search_values = local_search_values;
    return path;
}

Path ConnectionGraph ::  bestFirstSearch (unsigned int start,
                                            unsigned int goal)
{
  // initialize a vector to hold N copies of a value
//  search_values = vector<double>(nodes.size(), SEARCH_VALUE_NOT_SET);
    vector<double> local_search_values(nodes.size(), SEARCH_VALUE_NOT_SET);
    vector<unsigned int> previous_nodes(nodes.size(), PREVIOUS_NOT_SET);
    vector<unsigned int> previous_links(nodes.size(), 0);
    
  // add starting node here
    priority_queue<Prioritized> open;
     Vector3 start_position = nodes[start].position;
     Vector3 goal_position  = nodes[goal ].position;
     double start_heuristic = start_position.getDistanceXZ(goal_position);
     open.push(toPrioritized(start, start_heuristic));
    previous_nodes[start] = PREVIOUS_START;
   // unsigned int node_counter = 0;
  // process nodes here
    while(!open.empty())
     {
          // get a node ftom the open list
         unsigned int current = open.top().node_index;
         local_search_values[current] = open.top().priority;
          open.pop();
         
          // set display information here
//         search_values[current] = node_counter;
//                node_counter++;
         
          // check for goal reached here
                if(current == goal)
                {
                     // calculate the path here
                    // calculate the path
                               //Path path;
                               //path.start_index = start;
                                //49
                               //path.nodes.push_back(goal);
                    return calculatePath(previous_nodes, previous_links, local_search_values,current);
                }
         
          // add nodes to queue here
         for(unsigned int i = 0; i < nodes[current].links.size(); i++)
               {
                    unsigned int destination = nodes[current].links[i].destination;
                   if(previous_nodes[destination] == PREVIOUS_NOT_SET)
                               {
                                   Vector3 destination_position  = nodes[destination].position;
                                                    double heuristic = destination_position.getDistanceXZ(goal_position);
                                                    open.push(toPrioritized(destination, heuristic));
                                    previous_nodes[destination] = current;
                                   previous_links[destination] = i;
                               }
               }
     }  // end of while loop
    // no path exists
    Path path(start,local_search_values);
//    path.start_index = start;
//    path.search_values = local_search_values;
    return path;
}

Path ConnectionGraph ::  aStarSearch (unsigned int start,
                                            unsigned int goal)
{
  // initialize a vector to hold N copies of a value
  //search_values = vector<double>(nodes.size(), SEARCH_VALUE_NOT_SET);
    vector<double> local_search_values(nodes.size(), SEARCH_VALUE_NOT_SET);
    vector<unsigned int> previous_nodes(nodes.size(), PREVIOUS_NOT_SET);
    vector<unsigned int> previous_links(nodes.size(), 0);
    vector<double> given_costs(nodes.size(), 0.0);
    
  // add starting node here
    priority_queue<Prioritized> open;
     Vector3 start_position = nodes[start].position;
     Vector3 goal_position  = nodes[goal ].position;
     double start_heuristic = start_position.getDistanceXZ(goal_position);
     open.push(toPrioritized(start, start_heuristic));
    previous_nodes[start] = PREVIOUS_START;
    given_costs[start] = 0.0;
   // unsigned int node_counter = 0;
  // process nodes here
    while(!open.empty())
     {
          // get a node ftom the open list
         unsigned int current = open.top().node_index;
         if(local_search_values[current] == SEARCH_VALUE_NOT_SET)
             local_search_values[current] = open.top().priority;
          open.pop();
         
          // set display information here
//         search_values[current] = node_counter;
//                node_counter++;
         
          // check for goal reached here
                if(current == goal)
                {
                     // calculate the path here
                    // calculate the path
                               //Path path;
                               //path.start_index = start;
                                //49
                               //path.nodes.push_back(goal);
                     return calculatePath(previous_nodes, previous_links,local_search_values, current);
                }
         
          // add nodes to queue here
         for(unsigned int i = 0; i < nodes[current].links.size(); i++)
               {
                    unsigned int destination = nodes[current].links[i].destination;
                   double given_current = given_costs[current];
                               unsigned int link_type = nodes[current].links[i].link_type;
                               double traversal = getTraversalCost(link_type);
                               double given_destination = given_current + traversal;
                   if(previous_nodes[destination] == PREVIOUS_NOT_SET  || given_destination < given_costs[destination])
                               {
                                   Vector3 destination_position  = nodes[destination].position;
                                   double heuristic = destination_position.getDistanceXZ(goal_position);
                                   double new_priority = heuristic + given_destination;
                                   open.push(toPrioritized(destination, new_priority));
                                   previous_nodes[destination] = current;
                                   previous_links[destination] = i;
                                   given_costs[destination] = given_destination;
                               }
               }
     }  // end of while loop
    // no path exists
    Path path(start,local_search_values);
//    path.start_index = start;
//    path.search_values = local_search_values;
    return path;
}




Path ConnectionGraph :: calculatePath (const std::vector<unsigned int>& previous_nodes, const std::vector<unsigned int>& previous_links,const std::vector<double>& search_values,
                                       unsigned int current)
{
  // calculate the reverse path here
    vector<Link> reversed;
     unsigned int here = current;
     while(previous_nodes[here] != PREVIOUS_START)
     {
          unsigned int previous = previous_nodes[here];
         unsigned int link_index = previous_links[here];
                unsigned int link_type  = nodes[previous].links[link_index].link_type;
               reversed.push_back(toLink(here, link_type));
          here = previous;
     }
  // calculate the forward path here
    Path path(here,search_values);
      for(int i = reversed.size() - 1; i >= 0; i--)
      {
          path.appendLink(reversed[i]);
      }
  // return the forward path here
    return path;
}


void ConnectionGraph :: setColorForLink (unsigned int link_type) const
{

    if (link_type == LINK_TYPE_DOWN_FAR)
       glColor3d(1.0, 0.0, 1.0);  // Magenta for DownOnly
   else if (link_type == LINK_TYPE_DIAGONAL)
       glColor3d(0.0, 0.5, 1.0);  // Blue for Diagonal
   else if (link_type == LINK_TYPE_UP_1 || link_type == LINK_TYPE_DOWN_1)
       glColor3d(0.0, 1.0, 0.0);  // Green for Up1 and Down1
   else if (link_type == LINK_TYPE_JUMP2)
       glColor3d(1.0, 0.5, 0.0);  // Orange for Jump2
   else if (link_type == LINK_TYPE_JUMP3)
       glColor3d(1.0, 0.0, 0.0);  // Red for Jump3
   else if (link_type == LINK_TYPE_ADJACENT)
       glColor3d(1.0, 1.0, 0.0); // Yellow for Adjacent
   else
       glColor3d(0.0, 0.0, 1.0);
}




bool operator< (const Prioritized& lhs,
                const Prioritized& rhs)
{
  // we want low priorities to be better
  return lhs.priority > rhs.priority;
};


double ConnectionGraph :: getTraversalCost (unsigned int link_type) const
{
    if (link_type == LINK_TYPE_DOWN_FAR)
        return 3.0;
   else if (link_type == LINK_TYPE_DIAGONAL)
       return 1.4;
   else if (link_type == LINK_TYPE_UP_1)
       return 2.0;
    else if (link_type == LINK_TYPE_DOWN_1)
        return 1.0;
   else if (link_type == LINK_TYPE_JUMP2)
       return 4.0;
   else if (link_type == LINK_TYPE_JUMP3)
       return 5.0;  // Red for Jump3
   else if (link_type == LINK_TYPE_ADJACENT)
       return 1.0;

      return 1.0;
}
