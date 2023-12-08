#include <iostream>
#include <string>

class Common
{
public:
    static Common* getInstance();
    std::string FnFormatDateYYMMDD();
    std::string FnFormatDateYYMMDD_HHMMSS();

    Common(Common& common) = delete;

    void operator=(const Common&) = delete;

private:
    static Common* common_;
    Common();
};