#include <GLFW/glfw3.h>
#include <iostream>

#include "timer.h"

namespace game {

Timer::Timer(void)
{
    end_time_ = 0;
}


Timer::~Timer(void)
{
}


void Timer::Start(float end_time)
{
    double current_time = glfwGetTime();
    end_time_ = current_time + end_time;
}


bool Timer::Finished(void) const
{
    if (glfwGetTime() >= end_time_) {
        return true;
    }
    else {
        return false;
    }

}

} // namespace game
