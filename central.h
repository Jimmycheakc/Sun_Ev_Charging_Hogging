#pragma once

#include <iostream>
#include <string>
#include "Poco/Types.h"

class Central{

public:
    const std::string username = "testuser";
    const std::string password = "testpassword";
    const std::string centralServerIp = "192.168.2.226";
    const Poco::UInt16 centralServerPort = 9999;

    static Central* getInstance();
    bool FnSendHeartBeatUpdate();
    bool FnSendDeviceStatusUpdate();
    bool FnSendParkInParkOutInfo();

    Central(Central& central) = delete;

    void operator=(const Central&) = delete;

private:
    static Central* central_;
    Central();
};