#include "common.h"
#include "log.h"
#include "Poco/AutoPtr.h"
#include "Poco/Exception.h"
#include "Poco/File.h"
#include "Poco/FileChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"

AppLogger* AppLogger::logger_ = nullptr;

AppLogger::AppLogger()
{
    createLogFile();
}

AppLogger* AppLogger::getInstance()
{
    if (logger_ == nullptr)
    {
        logger_ = new AppLogger();
    }
    return logger_;
}

void AppLogger::createLogFile()
{
    Poco::File logDirectory(logFilePath);

    try
    {
        if (!logDirectory.exists())
        {
            logDirectory.createDirectories();
        }

        logDirectory.setWriteable(true);
    }
    catch(const Poco::Exception& ex)
    {
        std::cerr << "Error creating log directory : " << ex.displayText() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    Poco::AutoPtr<Poco::FileChannel> pChannel(new Poco::FileChannel);
    std::string absFilePath = logFilePath + "/ev_charging_hogging" + Common::getInstance()->FnGetDateTime() + ".log";
    pChannel->setProperty("path", absFilePath);
    //pChannel->setProperty("rotation", "1 M");

    Poco::AutoPtr<Poco::PatternFormatter> pPF(new Poco::PatternFormatter);
    pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S %s: %t");

    Poco::AutoPtr<Poco::FormattingChannel> pFC(new Poco::FormattingChannel(pPF, pChannel));
    Poco::Logger::root().setChannel(pFC);

    pocoLogger_ = &Poco::Logger::get("EvChargingHoggingLogger");
    
}

bool AppLogger::isLogFileExists()
{
    std::string absFilePath = logFilePath + "/ev_charging_hogging" + Common::getInstance()->FnGetDateTime() + ".log";
    Poco::File logFile(absFilePath);
    return logFile.exists();
}

void AppLogger::FnLog(const std::string& msg)
{
    if (!isLogFileExists())
    {
        createLogFile();
    }

    pocoLogger_->information(msg);
}