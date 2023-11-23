//
//  TimeManager.cpp
//  Tutorial 17
//
//  Created by Favour Olaifa-Olawale on 2023-10-25.
//

#include "TimeManager.h"

TimeManager::TimeManager()
    : UPDATES_PER_SECOND(60), MAX_UPDATES_PER_FRAME(10), DELTA_TIME(1000000 / UPDATES_PER_SECOND),MIN_SMOOTH_DURATION(5) {
    // Initialize timing variables
    start_time = system_clock::now();
    last_frame_time = start_time;
    last_frame_duration = DELTA_TIME;
    next_update_time = start_time;
    smoothed_frames_per_second = UPDATES_PER_SECOND;
    update_count = 0;
    frame_count = 0;
    instantaneous_updates_per_second = 0.0f;
    smoothed_updates_per_second = 0.0f;
}

TimeManager::TimeManager(int updates_per_second_in, unsigned int max_updates_per_frame_in)
    : UPDATES_PER_SECOND(updates_per_second_in), MAX_UPDATES_PER_FRAME(max_updates_per_frame_in),
      DELTA_TIME(1000000 / updates_per_second_in), MIN_SMOOTH_DURATION(5) {
    // Initialize timing variables
    start_time = system_clock::now();
    last_frame_time = start_time;
    last_frame_duration = DELTA_TIME;
    next_update_time = start_time;
    smoothed_frames_per_second = updates_per_second_in;
    update_count = 0;
    frame_count = 0;
    instantaneous_updates_per_second = 0.0f;
    smoothed_updates_per_second = 0.0f;
}

double TimeManager::getUpdateDeltaTime() const {
    return duration_cast<duration<double>>(DELTA_TIME).count();
}

unsigned int TimeManager::getMaxUpdatesPerFrame() const {
    return MAX_UPDATES_PER_FRAME;
}

bool TimeManager::isUpdateWaiting() const {
    return next_update_time < system_clock::now();
}

float TimeManager::getGameDuration() const {
    system_clock::time_point current_time = system_clock::now();
    return duration_cast<duration<float>>(current_time - start_time).count();
}

int TimeManager::getUpdateCount() const {
    return update_count;
}

int TimeManager::getFrameCount() const {
    return frame_count;
}

float TimeManager::getUpdateRateAverage() const {
    float game_duration = getGameDuration();
    return (game_duration > 0) ? static_cast<float>(update_count) / game_duration : 0.0f;
}

float TimeManager::getFrameRateAverage() const {
    float game_duration = getGameDuration();
    return (game_duration > 0) ? static_cast<float>(frame_count) / game_duration : 0.0f;
}

float TimeManager::getUpdateRateInstantaneous() const {
    return instantaneous_updates_per_second;
}

float TimeManager::getFrameRateInstantaneous() const {
    float last_frame_seconds = duration_cast<duration<float>>(last_frame_duration).count();
    return (last_frame_seconds > 0) ? 1.0f / last_frame_seconds : 0.0f;
}

float TimeManager::getUpdateRateSmoothed() const {
    return smoothed_updates_per_second;
}

float TimeManager::getFrameRateSmoothed() const {
    return smoothed_frames_per_second;
}

void TimeManager::sleepUntilNextUpdate() const {
    system_clock::time_point current_time = system_clock::now();
    
    if (current_time < next_update_time) {
        system_clock::duration sleep_time = next_update_time - current_time;
        sleep(duration<double>(sleep_time).count());
    }
}

void TimeManager::markNextUpdate() {
    update_count++;
    system_clock::time_point current_time = system_clock::now();
    //should be this
    //last_update_duration = current_time - next_update_time;
    //instead of this
    last_update_duration = current_time - last_update_time;
    if (last_update_duration < MIN_SMOOTH_DURATION)
        last_update_duration = MIN_SMOOTH_DURATION;
    last_update_time=current_time;
    
    
    // Calculate the instantaneous update rate
    float last_update_seconds = duration_cast<duration<float>>(last_update_duration).count();
    instantaneous_updates_per_second = 1.0f / last_update_seconds;

    // Calculate the smoothed update rate
    smoothed_updates_per_second = 0.95f * smoothed_updates_per_second + 0.05f * instantaneous_updates_per_second;
    
    next_update_time += DELTA_TIME;
}


void TimeManager::markNextFrame() {
    frame_count++;
    system_clock::time_point current_time = system_clock::now();
    last_frame_duration = current_time - last_frame_time;
    if (last_frame_duration < MIN_SMOOTH_DURATION)
        last_frame_duration = MIN_SMOOTH_DURATION;
    last_frame_time = current_time;
    float last_frame_seconds = duration_cast<duration<float>>(last_frame_duration).count();
    float instantaneous_frames_per_second = 1.0f / last_frame_seconds;
    smoothed_frames_per_second = 0.95f * smoothed_frames_per_second + 0.05f * instantaneous_frames_per_second;
}
