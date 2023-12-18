#pragma once

#include <iostream>
#include <memory>
#include "Poco/Timer.h"
#include "Poco/Thread.h"

class FilterTimer
{

public:
    static FilterTimer* getInstance();
    void FnStartFilterTimer();
    void FnStopFilterTimer();

    FilterTimer(FilterTimer& filterTimer) = delete;

    void operator=(const FilterTimer&) = delete;

private:
    static FilterTimer* filterTimer_;
    std::unique_ptr<Poco::Timer> pTimer_;
    FilterTimer();
    void onFilterTimerTimeout(Poco::Timer& timer);
};