#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
#include "database.h"
#include "ini_parser.h"
#include "Poco/Timer.h"
#include "Poco/Thread.h"
#include "log.h"
#include "timer.h"

EvtTimer* EvtTimer::evtTimer_ = nullptr;

EvtTimer::EvtTimer()
{
}

EvtTimer* EvtTimer::getInstance()
{
    if (evtTimer_ == nullptr)
    {
        evtTimer_ = new EvtTimer();
    }

    return evtTimer_;
}

void EvtTimer::onFilterTimerTimeout(Poco::Timer& timer)
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

    std::cout << "On Filter Timer called, time : " << buffer << std::endl;
}

void EvtTimer::FnStartFilterTimer()
{
    int interval = Iniparser::getInstance()->FnGetTimerForFilteringSnapShot() * 1000 * 60;
    pFilterTimer_ = std::make_unique<Poco::Timer>(interval, interval);

    // Register Timer callback
    pFilterTimer_->start(Poco::TimerCallback<EvtTimer>(*this, &EvtTimer::onFilterTimerTimeout));
}

void EvtTimer::FnStopFilterTimer()
{
    pFilterTimer_->stop();
}

void EvtTimer::onStartUpProcessTimerTimeout(Poco::Timer& timer)
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

    // Handling timeout case
    if (Database::getInstance()->FnIsTableEmpty("tbl_ev_lot_trans"))
    {
        
    }


    // Log the function timeout called
    std::ostringstream msg;
    msg << "On " << __func__ << " called, time : " << buffer << std::endl;
    AppLogger::getInstance()->FnLog(msg.str());
}

void EvtTimer::FnStartStartUpTimer()
{
    pStartUpProcessTimer_ = std::make_unique<Poco::Timer>(15000, 0);

    // Register Start Up Process Timer callback
    pStartUpProcessTimer_->start(Poco::TimerCallback<EvtTimer>(*this, &EvtTimer::onStartUpProcessTimerTimeout));
}

void EvtTimer::FnStopStartUpTimer()
{
    pStartUpProcessTimer_->stop();
}
