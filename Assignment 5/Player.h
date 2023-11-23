//
//  Player.h
//

#pragma once

#include "ObjLibrary/Vector3.h"



const double PLAYER_HALF_HEIGHT = 0.8;

class Player
{
public:
	Player ();

	const ObjLibrary::Vector3& getPosition () const;
    const ObjLibrary::Vector3& getVelocity () const;
	double getYawDegrees () const;
	ObjLibrary::Vector3 getForward () const;
	ObjLibrary::Vector3 getRight () const;

	void reset (const ObjLibrary::Vector3& start_position);
	void addPosition (const ObjLibrary::Vector3& delta);
    void setPosition (const ObjLibrary::Vector3& delta);
    void addVelocity (const ObjLibrary::Vector3& delta);
    void setVelocity (const ObjLibrary::Vector3& delta);
	void addYawDegrees (double delta);
    void setYawDegrees (double delta);
    void setPitchDegrees (double delta);
	void addPitchDegrees (double delta);
    void updatePosition(double delta_time);
    void  accelerateToVelocity(double max_speed, double factor_per_update,ObjLibrary::Vector3 desired);
    void accelerateToNode(double max_speed, double factor_per_update,ObjLibrary::Vector3 nodePosition);
    
   // double PitchCal(const Vector3& Thrower,const Vector3& Target, double max_speed );
   // double PitchCal( ObjLibrary::Vector3 Target, double max_speed );
    

private:
	ObjLibrary::Vector3 position;
	double yaw_degrees;
	double pitch_degrees;
    ObjLibrary::Vector3 velocity;
};
