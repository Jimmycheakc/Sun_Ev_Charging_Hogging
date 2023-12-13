#include <iostream>
#include <string>
#include <sstream>
#include "http_client.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPDigestCredentials.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/MultipartReader.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/NumberParser.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "common.h"
#include "log.h"

httpClient* httpClient::httpClient_ = nullptr;

httpClient::httpClient()
{
    createImageDirectory();
}

httpClient* httpClient::getInstance()
{
    if (httpClient_ == nullptr)
    {
        httpClient_ = new httpClient();
    }

    return httpClient_;
}

void httpClient::createImageDirectory()
{
    Poco::File imageDirectory(imageDirectoryPath);

    try
    {
        if (!imageDirectory.exists())
        {
            imageDirectory.createDirectories();
        }

        imageDirectory.setWriteable(true);
    }
    catch(const Poco::Exception& ex)
    {
        std::cerr << "Error creating image directory : " << ex.displayText() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    
}

bool httpClient::isImageDirectoryExists()
{
    Poco::File imageDirectory(imageDirectoryPath);
    return imageDirectory.exists();
}

bool httpClient::do_heartBeatRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);
    
    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());
    
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::ostringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);
        AppLogger::getInstance()->FnLog(responseStream.str());

        // Temp: Handle the response

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool httpClient::FnGetHeartBeat()
{
    const std::string uri_link= "http://" + cameraServerIP + "/cgi-bin/trafficParking.cgi?action=getAllParkingSpaceStatus";

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        Poco::Net::HTTPDigestCredentials credentials(username, password);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        if (!do_heartBeatRequest(session, request, response))
        {
            credentials.authenticate(request, response);
            if (!do_heartBeatRequest(session, request, response))
            {
                AppLogger::getInstance()->FnLog("Invalid username or password");
                return false;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }

    return true;
}

bool httpClient::do_snapShotRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);

    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());

    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::ostringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);

        // Save the image
        std::string imageData = responseStream.str();
        std::string absImagePath = imageDirectoryPath + "/Img_" + Common::getInstance()->FnFormatDateYYMMDD_HHMMSS() + ".jpg";
        if (!isImageDirectoryExists())
        {
            createImageDirectory();
        }
        Poco::FileOutputStream fileStream(absImagePath);
        fileStream << imageData;
        fileStream.close();

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool httpClient::FnGetSnapShot()
{
    const std::string uri_link= "http://" + cameraServerIP + "/cgi-bin/snapshot.cgi?channel=1&type=0";

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        Poco::Net::HTTPDigestCredentials credentials(username, password);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        if (!do_snapShotRequest(session, request, response))
        {
            credentials.authenticate(request, response);
            if (!do_snapShotRequest(session, request, response))
            {
                AppLogger::getInstance()->FnLog("Invalid username or password");
                return false;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }

    return true;
}

bool httpClient::do_subscribeToSnapShotParked(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);
    
    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());
    
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::ostringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);
        AppLogger::getInstance()->FnLog(responseStream.str());

        // Temp: Handle the response

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool httpClient::FnSubscibeToSnapShotParked()
{
    const std::string uri_link= "http://" + cameraServerIP + "/cgi-bin/snapManager.cgi?action=attachFileProc&channel=1&heartbeat=5&Flags[0]=Event&Events=[TrafficParkingSpaceParking]";

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        Poco::Net::HTTPDigestCredentials credentials(username, password);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        if (!do_subscribeToSnapShotParked(session, request, response))
        {
            credentials.authenticate(request, response);
            if (!do_subscribeToSnapShotParked(session, request, response))
            {
                AppLogger::getInstance()->FnLog("Invalid username or password");
                return false;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }

    return true;
}

bool httpClient::do_subscribeToSnapShot(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);
    
    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());
    
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::string contentType = response.get("Content-Type", "");
        std::string boundary;
        Poco::Net::NameValueCollection params;

        Poco::Net::MessageHeader::splitParameters(contentType.begin(), contentType.end(), params);
        boundary = params.get("boundary", "");
        Poco::Net::MultipartReader reader(rs, boundary);
        event_t event;
        memset(&event, 0 , sizeof(event));

        while (reader.hasNextPart())
        {
            Poco::Net::MessageHeader partHeader;
            reader.nextPart(partHeader);

            std::string ct = partHeader.get("Content-Type", "");
            std::string cl = partHeader.get("Content-Length", "");

            if (!cl.empty() && !ct.empty())
            {
                int contentLength = std::stoi(cl);
                if (ct == "text/plain") {
                    // Process text/plain part
                    std::string partData;
                    Poco::StreamCopier::copyToString(reader.stream(), partData);
                    std::istringstream partStream(partData);
                    std::string line;
                    memset(&event, 0 , sizeof(event));

                    while (std::getline(partStream, line))
                    {
                        if (line.find("Heartbeat") != std::string::npos)
                        {
                            AppLogger::getInstance()->FnLog(line);
                        }
                        else if (line.find("].Channel") != std::string::npos)
                        {
                            size_t equalSignPos = line.find("=");
                            if (equalSignPos != std::string::npos)
                            {
                                event.evt_channel = line.substr(equalSignPos+1, line.length() - equalSignPos);
                            }
                            AppLogger::getInstance()->FnLog(line);
                        }
                        else if (line.find("].ParkingSpaceStatus") != std::string::npos)
                        {
                            size_t equalSignPos = line.find("=");
                            if (equalSignPos != std::string::npos)
                            {
                                event.evt_parking_status = line.substr(equalSignPos+1, line.length() - equalSignPos);
                            }
                            AppLogger::getInstance()->FnLog(line);
                        }
                        else if (line.find("].TrafficCar.PlateNumber") != std::string::npos)
                        {
                            size_t equalSignPos = line.find("=");
                            if (equalSignPos != std::string::npos)
                            {
                                event.evt_lpn = line.substr(equalSignPos+1, line.length() - equalSignPos);
                            }
                            AppLogger::getInstance()->FnLog(line);
                        }
                        else if (line.find("].TrafficCar.UTC") != std::string::npos)
                        {
                            size_t equalSignPos = line.find("=");
                            if (equalSignPos != std::string::npos)
                            {
                                event.evt_snapshot_time = line.substr(equalSignPos+1, line.length() - equalSignPos);
                            }
                            AppLogger::getInstance()->FnLog(line);
                        }
                        else if (line.find("].TrafficCar.Lane") != std::string::npos)
                        {
                            size_t equalSignPos = line.find("=");
                            if (equalSignPos != std::string::npos)
                            {
                                event.evt_lane = line.substr(equalSignPos+1, line.length() - equalSignPos);
                            }
                            AppLogger::getInstance()->FnLog(line);
                        }
                    }
                }
                else if (ct == "image/jpeg")
                {
                    try
                    {
                        // Save the image
                        std::string absImagePath;

                        if (!event.evt_snapshot_time.empty())
                        {
                            try
                            {
                                std::string UTCStr = event.evt_snapshot_time;
                                Poco::trimInPlace(UTCStr);
                                Poco::Int64 intUTC = Poco::NumberParser::parse(UTCStr);
                                Poco::DateTime dateTimeUTC(Poco::Timestamp::fromEpochTime(intUTC));
                                std::string dateTimeUTCStr(Poco::DateTimeFormatter::format(dateTimeUTC, "%y%m%d_%H%M%S"));
                                absImagePath = imageDirectoryPath + "/Img_" + dateTimeUTCStr + ".jpg";
                            }
                            catch(const Poco::Exception& ex)
                            {
                                std::cerr << "Error Parsing the string as integer : " << ex.displayText() << std::endl;
                            }
                        }
                        else
                        {
                            absImagePath = imageDirectoryPath + "/Img_" + Common::getInstance()->FnFormatDateYYMMDD_HHMMSS() + ".jpg";
                            AppLogger::getInstance()->FnLog("Date for filename is get from current time.");
                        }

                        if (!isImageDirectoryExists())
                        {
                            createImageDirectory();
                        }
                        Poco::FileOutputStream fileStream(absImagePath);
                        Poco::StreamCopier::copyStream(reader.stream(), fileStream);
                        fileStream.close();
                        AppLogger::getInstance()->FnLog("Image stored :" + absImagePath);
                    }
                    catch(Poco::Exception& ex)
                    {
                        std::cerr << "Error saving image: " << ex.displayText() << std::endl;
                    }
                }
            }
        }

        session.reset();
        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool httpClient::FnSubscribeToSnapShot()
{
    const std::string uri_link= "http://" + cameraServerIP + "/cgi-bin/snapManager.cgi?action=attachFileProc&channel=1&heartbeat=60&Flags[0]=Event&Events=[TrafficParkingSpaceParking%2CTrafficParkingSpaceNoParking]";

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        Poco::Net::HTTPDigestCredentials credentials(username, password);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        if (!do_subscribeToSnapShot(session, request, response))
        {
            credentials.authenticate(request, response);
            if (!do_subscribeToSnapShot(session, request, response))
            {
                AppLogger::getInstance()->FnLog("Invalid username or password");
                return false;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }

    return true;
}

bool httpClient::do_getCurrentTime(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response, std::string& dateTime)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);
    
    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());
    
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::ostringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);
        dateTime = responseStream.str();
        AppLogger::getInstance()->FnLog(responseStream.str());

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool httpClient::FnGetCurrentTime(std::string& dateTime)
{
    const std::string uri_link= "http://" + cameraServerIP + "/cgi-bin/global.cgi?action=getCurrentTime";

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        Poco::Net::HTTPDigestCredentials credentials(username, password);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        if (!do_getCurrentTime(session, request, response, dateTime))
        {
            credentials.authenticate(request, response);
            if (!do_getCurrentTime(session, request, response, dateTime))
            {
                AppLogger::getInstance()->FnLog("Invalid username or password");
                return false;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }

    return true;
}

bool httpClient::do_setCurrentTime(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);
    
    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());
    
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::ostringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);
        AppLogger::getInstance()->FnLog(responseStream.str());
        
        if (responseStream.str().compare("OK"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(rs, null);
        return false;
    }
}

bool httpClient::FnSetCurrentTime()
{
    Poco::LocalDateTime now;
    std::string dateTimeStr(Poco::DateTimeFormatter::format(now, "%Y-%n-%e%%20%H:%M:%S"));

    const std::string uri_link= "http://" + cameraServerIP + "/cgi-bin/global.cgi?action=setCurrentTime&time=" + dateTimeStr;

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        Poco::Net::HTTPDigestCredentials credentials(username, password);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        if (!do_setCurrentTime(session, request, response))
        {
            credentials.authenticate(request, response);
            if (!do_setCurrentTime(session, request, response))
            {
                AppLogger::getInstance()->FnLog("Invalid username or password");
                return false;
            }
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }

    return true;
}
