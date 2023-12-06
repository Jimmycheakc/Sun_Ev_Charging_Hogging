#include <iostream>
#include <string>
#include "common.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"

Common* Common::common_ = nullptr;

Common::Common()
{
}

Common* Common::getInstance()
{
    if (common_ == nullptr)
    {
        common_ = new Common();
    }

    return common_;
}

std::string Common::FnGetDateTime()
{
    Poco::DateTime now;
    std::string dateTimeStr(Poco::DateTimeFormatter::format(now, "%y%m%d"));
    return dateTimeStr;
}