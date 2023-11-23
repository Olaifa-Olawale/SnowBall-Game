//
//  Player.cpp
//

#include "Player.h"

#include "ObjLibrary/Vector3.h"

#include "Angles.h"

using namespace ObjLibrary;



Player :: Player ()
	: position(0.0, PLAYER_HALF_HEIGHT, 0.0),
	  yaw_degrees(0.0),
	  pitch_degrees(0.0)
{
}


const ObjLibrary::Vector3& Player :: getPosition () const
{
    return position;
}

const ObjLibrary::Vector3& Player :: getVelocity () const
{
	return velocity;
}

double Player :: getYawDegrees () const
{
	return yaw_degrees;
}

ObjLibrary::Vector3 Player :: getForward () const
{
	Vector3 forward = Vector3::UNIT_X_PLUS;
	forward.rotateZ(degreesToRadians(pitch_degrees));
	forward.rotateY(degreesToRadians(yaw_degrees));
	return forward;
}

ObjLibrary::Vector3 Player :: getRight () const
{
	Vector3 forward = Vector3::UNIT_Z_PLUS;
	forward.rotateY(degreesToRadians(yaw_degrees));
	return forward;
}



void Player :: reset (const ObjLibrary::Vector3& start_position)
{
	position = start_position;
	yaw_degrees   = 0.0;
	pitch_degrees = 0.0;
}

void Player :: addPosition (const Vector3& delta)
{
    position += delta;
}

void Player :: setPosition (const Vector3& delta)
{
    position = delta;
}

void Player :: addVelocity (const Vector3& delta)
{
    velocity += delta;
}

void Player :: setVelocity (const Vector3& delta)
{
	velocity = delta;
}

void Player :: addYawDegrees (double delta)
{
    yaw_degrees += delta;
}

void Player :: setYawDegrees (double delta)
{
    yaw_degrees = delta;
}

void Player :: setPitchDegrees (double delta)
{
	pitch_degrees = delta;
}


void Player :: addPitchDegrees (double delta)
{
	static const double PITCH_MIN = -60.0;
	static const double PITCH_MAX =  60.0;

	pitch_degrees += delta;
	if(pitch_degrees < PITCH_MIN)
		pitch_degrees = PITCH_MIN;
	if(pitch_degrees > PITCH_MAX)
		pitch_degrees = PITCH_MAX;
}

void Player ::  updatePosition(double delta_time) {
        // Update the position using velocity and delta-time
        position += velocity * delta_time;
    
}

void Player :: accelerateToVelocity(double max_speed, double factor_per_update,Vector3 desired) {
        // Calculate the horizontal component of the forward vector
         desired.y = 0.0;
    
        // Set its length to the maximum speed
        desired.setNormSafe(max_speed);

        // Get the current horizontal velocity
        Vector3 current = getVelocity();
        current.y = 0.0;


        // Interpolate the horizontal velocity
        Vector3 combined = desired * factor_per_update + current * (1.0 - factor_per_update);
        combined.y = getVelocity().y;

        // Set the new velocity
        setVelocity(combined);
    }

void Player :: accelerateToNode(double max_speed, double factor_per_update,Vector3 desired) {
        
    // Calculate the vector pointing towards the desired node
    Vector3 towardsNode = desired - getPosition();
    // Calculate the horizontal component of the forward vector
//    std::cout<<"Second node pos: "<<desired<<std::endl;
//    std::cout<<"Opponent Position : "<<getPosition()<<std::endl;w
    //desired.normalize();
    towardsNode.y = 0.0;
   
        
        // Set its length to the maximum speed
    towardsNode.setNormSafe(max_speed);

        // Get the current horizontal velocity
        Vector3 current = getVelocity();
        current.y = 0.0;


        // Interpolate the horizontal velocity
        Vector3 combined = towardsNode * factor_per_update + current * (1.0 - factor_per_update);
        combined.y = getVelocity().y;

        // Set the new velocity
        setVelocity(combined);
    //std::cout<<"1"<<getVelocity()<<std::endl;
    
    }

//double Player :: PitchCal ( Vector3 Target, double max_speed )
//{
//    double x = position.getDistanceXZ(Target);
//    double y = Target.y-position.y;
//    double v = 15.0;
//    double F = -9.8;
//   // double tita = radiansToDegrees(opponent.getYawDegrees());
////
//    double A = pow(v,2);
//    double B = pow(v,4);
//    double C = 2 * y * A;
//    double D = F * pow(x,2);
//    double E = pow((B + (F * (C-D))),0.5);
//    double G = A - E;
//    double  H = G/(F*x);
//    return  atan(H);
//    
//}
