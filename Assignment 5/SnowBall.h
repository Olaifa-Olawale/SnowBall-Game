//
//  SnowBall.hpp
//  Assignment 4
//
//  Created by Favour Olaifa-Olawale on 2023-10-30.
//

#ifndef SnowBall_h
#define SnowBall_h

#include <stdio.h>
#include "Vector3.h"
#include "Player.h"
#include "CollisonBox.h" // Assuming you have a CollisionBox class
#include "Angles.h"
using namespace ObjLibrary;
using namespace std;


class Snowball {
public:
    Box snowBall_Box;
    Snowball():
    SnowBallVelocity(Vector3(15.0, 15.0, 15.0))
    {
        
    }
    void setVelocity (const Vector3& delta)
    {
        SnowBallVelocity = delta;
    }
    void  setPosition (const Vector3& delta)
    {
        snowBall_Box.center = delta;
    }
    
//    bool  getThrower ()
//    {
//        return WhoThrew;
//    }
//    
    const Vector3&  getVelocity () const
    {
        return SnowBallVelocity;
    }

    const ObjLibrary::Vector3& getPosition () const
    {
        return snowBall_Box.center;
    }
    
    void ThrowSnowBall(const Player& player) {
        SnowBallVelocity = player.getForward()*15;

        snowBall_Box.center= player.getPosition();
        snowBall_Box.center.y += 0.6;
        snowBall_Box.center += player.getForward() * 0.25;
        
    }
    
    double PitchCal(const Player& opp,const Player& player )
    {
        double x = opp.getPosition().getDistanceXZ(player.getPosition());
        double y = player.getPosition().y-opp.getPosition().y;
        double v = 15.0;
        double F = -9.8;
        double A = pow(v,2);
        double B = pow(v,4);
        double C = 2 * y * A;
        double D = F * pow(x,2);
        double E = B + (F * (C-D));
        //cout<<"E is : "<<E<<endl;
//        cout<<"X is : "<<x<<endl;
//        cout<<"Y is : "<<y<<endl;
        if(E<0)
        {
            cout<<"bad number"<<endl;
            return 90;
            
        }
        double E2 = pow(E,0.5);
        double G = A - E2;
        double  H = G/(F*x);
       
        return  -atan(H);
    
    }
    
    bool ThrowOppSnowBall(Player& opp,const Player& player) {
    
        double pitch = radiansToDegrees(PitchCal( opp,player));
        //cout<<"pitch is "<<pitch<<endl;
        if (pitch < -60.0 || pitch > 60)
            return false;
        
        
        Vector3 relative = player.getPosition() - opp.getPosition();
        double yaw_radians = relative.getRotationYSafe();
//        cout<<"Realative : "<<relative<<endl;
//        cout<<"Yaw Radians: "<<yaw_radians<<endl;
        opp.setPitchDegrees(pitch);
        opp.setYawDegrees(radiansToDegrees(yaw_radians)-90);
      //  WhoThrew=false;
        ThrowSnowBall(opp);
       // cout<<"Yaw is : "<<opp.getYawDegrees()<<endl;
        return true;
    }
    
    void SnowBallUpdate(double delta) {
        // Apply gravity
         const double gravity = -9.81;
         SnowBallVelocity.y += gravity * delta;
        
        // Move the snowball according to its velocity
        snowBall_Box.center+= SnowBallVelocity * delta;

        
       }
    
    void initSnowBallCollisionBoxes()
    {
        snowBall_Box.half_size= Vector3(0.1,0.1,0.1);
        snowBall_Box.is_colliding=false;
    }
    
   
private:
    
   
    Vector3 SnowBallVelocity;
  
  // bool WhoThrew;//false for opponent
                //true for player
    
   
  
};


#endif /* SnowBall_hpp */
