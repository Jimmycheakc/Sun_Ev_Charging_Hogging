#include <iostream>
#include <string>
#include "Poco/AutoPtr.h"
#include "Poco/Logger.h"

class AppLogger
{
public:
    const std::string logFilePath = "/var/log/ev_charging_hogging";

    static AppLogger* getInstance();
    void FnLog(const std::string& msg);

    AppLogger(AppLogger& logger) = delete;

    void operator=(const AppLogger&) = delete;

private:
    static AppLogger* logger_;
    Poco::AutoPtr<Poco::Logger> pocoLogger_;
    AppLogger();
    void createLogFile();
    bool isLogFileExists();
};