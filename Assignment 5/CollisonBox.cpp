//
//  CollisonBox.cpp
//  Assignment 3
//
//  Created by Favour Olaifa-Olawale on 2023-10-06.
//

#include "CollisonBox.h"

void Box::drawBox (const Box& box)
{
   glColor3d(0.0, 0.0, 1.0);  // blue
    glPushMatrix();
  
        glTranslated(box.center.x, box.center.y, box.center.z);
        // add glScaled here
        glScaled(box.half_size.x, box.half_size.y, box.half_size.z);
    //glScaled(box.half_size.x+0.01, box.half_size.y+0.01, box.half_size.z+0.01);
        glutWireCube(2.0);  // from -1.0 to +1.0
    glPopMatrix();
}


void Box::drawBlockMap (const Box& box)
{
    glColor3d(0.0, 0.0, 1.0);  // blue
    glPushMatrix();
    glTranslated(box.center.x, box.center.y, box.center.z);
    glScaled(box.half_size.x+0.01, box.half_size.y+0.01, box.half_size.z+0.01);
    glutWireCube(2.0);  // from -1.0 to +1.0
glPopMatrix();
}

