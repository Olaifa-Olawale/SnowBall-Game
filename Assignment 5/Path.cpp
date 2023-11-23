//
//  Path.cpp
//  Tutorial 21
//
//  Created by Favour Olaifa-Olawale on 2023-11-13.
//

#include "Path.h"
#include "ConnectionGraph.h"

Path::Path() : start_index(0),link_timer(0.0) {
    // Default constructor
}

Path::Path(unsigned int start_index_in, const std::vector<double>& search_values_in)
    : start_index(start_index_in), search_values(search_values_in),link_timer(0.0) {
    // Constructor with parameters
}

bool Path::isEmpty() const {
    return links.empty();
}

const ObjLibrary::Vector3& Path::getStartPosition(const ConnectionGraph& graph) const {
    return graph.getNodePosition(start_index);
}



void Path::resetLinkTimer() {
        link_timer = 0.0;
    }

    void Path::updateLinkTimer(double delta_time) {
        link_timer += delta_time;
    }

    double Path::getLinkTimer() const {
        return link_timer;
    }

void Path::drawPath(const ConnectionGraph& graph) const {
std::queue<Link> copy_queue = links;
Vector3 start = graph.getNodePosition(start_index);
// draw the path segments here
  Vector3 previous = start;
    glLineWidth(2.0);
    glBegin(GL_LINES);
         //for(unsigned int i = 0; i < links.size(); i++)
    while (!copy_queue.empty()) {
           Link link = copy_queue.front();
           copy_queue.pop();
              unsigned int current_index = link.destination;
              Vector3 current = graph.getNodePosition(current_index);
              //glColor3d(0.0, 1.0, 1.0);
              graph.setColorForLink(link.link_type);
              glVertex3d(previous.x, previous.y + 0.2, previous.z);
              glVertex3d( current.x,  current.y + 0.2,  current.z);
              previous = current;
         }
    glEnd();
    glLineWidth(1.0);
}

void Path::appendLink(const Link& link) {
    links.push(link);
}


void Path:: drawSearchValues (ConnectionGraph& graph) const
{
  // calculate minimum and maximum values here
    
    double value_min =  1.0e20;  // very large
     double value_max = -1.0e20;  // very small
     for(unsigned int i = 0; i < graph.getNodeCount(); i++)
     {
          double value = search_values[i];
          if(value != SEARCH_VALUE_NOT_SET)
          {
               if(value < value_min)
                    value_min = value;
               if(value > value_max)
                    value_max = value;
          }
     }
  // range calculations here
 
    if(value_min > value_max)
          return;  // no meaningful values to draw
     double value_range = value_max - value_min;
   
    if(value_range == 0.0)
           value_range = 1.0;
  // display values here
    glPointSize(10.0);
    glBegin(GL_POINTS);
           for(unsigned int i = 0; i < graph.getNodeCount(); i++)
           {
                double value = search_values[i];
                if(value != SEARCH_VALUE_NOT_SET)
                {
                     // set color here
                    double fraction = (value - value_min) / value_range;
                        glColor3d(1.0, fraction, 0.0);
                     Vector3 position = graph.getNodePosition(i);
                     glVertex3d(position.x, position.y + 0.2, position.z);
                 }
           }
      glEnd();
}




const ObjLibrary::Vector3& Path::getSecondNodePosition(const ConnectionGraph& graph) const {
    // Check if there's at least one link in the path
    if (!links.empty()) {
        unsigned int second_node_index = links.front().destination;
        //cout<<"second_node_index: "<<second_node_index<<endl;
        return graph.getNodePosition(second_node_index);
    }
    return graph.getNodePosition(start_index);
}



unsigned int Path::getLinkTypeToSecondNode() const {
  
    if (!links.empty()) {
        return links.front().link_type;
    }

    return 0;
}



void Path::removeFirstLinkAndSetNewStartNode() {
    // Check if there's at least one link in the path
    if (!links.empty()) {
        // Set the new start node to the destination of the first link
        start_index = links.front().destination;
      //  cout<<"Removed"<<endl;
        
        links.pop();
    }
    else
        cout<<"No more links"<<endl;
    
}



void Path::FrontToAdjacent()
{
    links.front().link_type=LINK_TYPE_ADJACENT;

}

void Path::destroyPath() {
        
    //Link=link.empty();
        // ... other destruction logic ...
//    cout<<""<<ENDL;
//    cout<<"hi"<<endl;
    
  //  links.clear();
    }
