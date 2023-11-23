//
//  CollisonBox.hpp
//  Assignment 3
//
//  Created by Favour Olaifa-Olawale on 2023-10-06.
//

#ifndef CollisonBox_h
#define CollisonBox_h
#define GL_SILENCE_DEPRECATION
#include "Vector3.h"
#include "GetGlut.h"


using namespace ObjLibrary;
using namespace std;


struct Box
{
    Vector3 center;
    Vector3 half_size;
    bool is_colliding;
    //bool isplayerBox;
    void drawBox (const Box& box);
    void drawBlockMap (const Box& box);

};


#include <stdio.h>

#endif /* CollisonBox_hpp */
