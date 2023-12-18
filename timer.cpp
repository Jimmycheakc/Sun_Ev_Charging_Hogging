#include <chrono>
#include <iostream>
#include <memory>
#include "ini_parser.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "timer.h"

FilterTimer* FilterTimer::filterTimer_ = nullptr;

FilterTimer::FilterTimer()
{
}

FilterTimer* FilterTimer::getInstance()
{
    if (filterTimer_ == nullptr)
    {
        filterTimer_ = new FilterTimer();
    }

    return filterTimer_;
}

void FilterTimer::onFilterTimerTimeout(Poco::Timer& timer)
{
    // Get the current time_point
    auto currentTime = std::chrono::system_clock::now();

    // Convert time_point to time_t
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);

    // Convert time_t to tm structure
    std::tm* currentTimeStruct = std::localtime(&currentTime_t);

    // Format the time as a string
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", currentTimeStruct);

    std::cout << "On Timer called, time : " << buffer << std::endl;
}

void FilterTimer::FnStartFilterTimer()
{
    int interval = Iniparser::getInstance()->FnGetTimerForFilteringSnapShot() * 1000;
    std::cout << "Interval : " << interval << std::endl;
    pTimer_ = std::make_unique<Poco::Timer>(0, interval);

    // Register Timer callback
    pTimer_->start(Poco::TimerCallback<FilterTimer>(*this, &FilterTimer::onFilterTimerTimeout));
}

void FilterTimer::FnStopFilterTimer()
{
    pTimer_->stop();
}
