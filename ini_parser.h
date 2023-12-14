#pragma once

#include <iostream>
#include <string>
#include "Poco/AutoPtr.h"
#include "Poco/Util/IniFileConfiguration.h"

class Iniparser{

public:
    static Iniparser* getInstance();
    void FnIniParserInit();
    std::string FnGetParkingLotLocationCode();
    int FnGetTimerForFilteringSnapShot();

    Iniparser(Iniparser& iniparser) = delete;

    void operator=(const Iniparser&) = delete;

private:
    static Iniparser* iniparser_;
    Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf_;
    std::string parkingLotLocationCode_;
    int timerForFilteringSnapShot_;
    Iniparser();
};