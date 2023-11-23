//
//  TimeManager.hpp
//  Tutorial 17
//
//  Created by Favour Olaifa-Olawale on 2023-10-25.
//

#ifndef TimeManager_h
#define TimeManager_h

#include <stdio.h>
#include <chrono>
#include "Sleep.h"

using namespace std;
using namespace chrono;

class TimeManager {
public:
    TimeManager ();
    TimeManager (int updates_per_second_in,
    unsigned int max_updates_per_frame_in);
    double getUpdateDeltaTime () const;
    unsigned int getMaxUpdatesPerFrame () const;
    bool isUpdateWaiting () const;
    float getGameDuration () const;
    int getUpdateCount () const;
    int getFrameCount () const;
    float getUpdateRateAverage () const;
    float getFrameRateAverage () const;
    float getUpdateRateInstantaneous () const;
    float getFrameRateInstantaneous () const;
    float getUpdateRateSmoothed () const;
    float getFrameRateSmoothed () const;
    void sleepUntilNextUpdate () const;
    void markNextUpdate ();
    void markNextFrame ();
    
private:
//    const int UPDATES_PER_SECOND = 60;
    const int UPDATES_PER_SECOND ;
    //const microseconds DELTA_TIME(1000000 / UPDATES_PER_SECOND);
    const microseconds DELTA_TIME;
    system_clock::time_point last_frame_time;
    duration<float> last_frame_duration;
    system_clock::time_point start_time;
    float smoothed_frames_per_second;
    /*float smoothed_frames_per_second = UPDATES_PER_SECOND;*/
    int update_count = 0;
    /* int update_count = 0;*/
    system_clock::time_point last_update_time;
    duration<float> last_update_duration;
    float instantaneous_updates_per_second ;
    float smoothed_updates_per_second ;
    system_clock::time_point next_update_time;
    const milliseconds MIN_SMOOTH_DURATION;
    /*const milliseconds MIN_SMOOTH_DURATION(5);*/
    const unsigned int MAX_UPDATES_PER_FRAME ;
    /*const unsigned int MAX_UPDATES_PER_FRAME = 10;*/
    int frame_count;
};


#endif /* TimeManager_hpp */
