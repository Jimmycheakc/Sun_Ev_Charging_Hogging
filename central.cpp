#include <iostream>
#include <string>
#include <sstream>
#include "central.h"
#include "common.h"
#include "ini_parser.h"
#include "log.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Exception.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"

Central* Central::central_ = nullptr;
const std::string Central::ERROR_CODE_RECOVERED = "0";
const std::string Central::ERROR_CODE_CAMERA = "1";
const std::string Central::ERROR_CODE_IPC = "2";

Central::Central()
{
    centralServerIp = Iniparser::getInstance()->FnGetCentralIP();
    centralServerPort = Iniparser::getInstance()->FnGetCentralServerPort();
}

Central* Central::getInstance()
{
    if (central_ == nullptr)
    {
        central_ = new Central();
    }
    return central_;
}

bool Central::doSendHeartBeatUpdate(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    request.setContentType("application/json");

    Poco::JSON::Object::Ptr jsonObject = new Poco::JSON::Object;
    jsonObject->set("username", username);
    jsonObject->set("password", password);
    jsonObject->set("carpark_code", Iniparser::getInstance()->FnGetParkingLotLocationCode());
    jsonObject->set("heartbeat_dt", Common::getInstance()->FnCurrentFormatDateYYYY_MM_DD_HH_MM_SS());
    jsonObject->set("msg", "Hearbeat Update");

    std::ostringstream jsonStringStream;
    jsonObject->stringify(jsonStringStream);
    request.setContentLength(jsonStringStream.str().length());

    // Send the request
    std::ostream& requestStream = session.sendRequest(request);
    requestStream << jsonStringStream.str();

    // Receive the response
    std::istream& responseStream = session.receiveResponse(response);

    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());

    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
    {
        std::ostringstream rs;
        Poco::StreamCopier::copyStream(responseStream, rs);
        AppLogger::getInstance()->FnLog(rs.str());

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(responseStream, null);

        return false;
    }
}

bool Central::FnSendHeartBeatUpdate()
{
    const std::string uri_link = "http://" + centralServerIp;
    bool ret = false;

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        
        Poco::Net::HTTPClientSession session(uri.getHost(), centralServerPort);
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/HeartBeat", Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        int retry = 0;

        if (!doSendHeartBeatUpdate(session, request, response))
        {
            retry = 3;

            while (retry > 0)
            {
                if (doSendHeartBeatUpdate(session, request, response))
                {
                    ret = true;
                    break;
                }
                else
                {
                    retry--;
                }
            }
        }
        else
        {
            ret = true;
        }

    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        ret = false;
    }
    
    return ret;
}

bool Central::doSendDeviceStatusUpdate(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response, const std::string& deviceIP, const std::string& ec)
{
    AppLogger ::getInstance()->FnLog(request.getURI());

    request.setContentType("application/json");

    Poco::JSON::Object::Ptr jsonObject = new Poco::JSON::Object;
    jsonObject->set("username", username);
    jsonObject->set("password", password);
    jsonObject->set("carpark_code", Iniparser::getInstance()->FnGetParkingLotLocationCode());
    jsonObject->set("device_ip", deviceIP);
    jsonObject->set("error_code", ec);
    
    std::ostringstream jsonStringStream;
    jsonObject->stringify(jsonStringStream);
    request.setContentLength(jsonStringStream.str().length());

    // Send the request
    std::ostream& requestStream = session.sendRequest(request);
    requestStream << jsonStringStream.str();

    // Receive the response
    std::istream& responseStream = session.receiveResponse(response);

    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());

    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
    {
        std::ostringstream rs;
        Poco::StreamCopier::copyStream(responseStream, rs);
        AppLogger::getInstance()->FnLog(rs.str());

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(responseStream, null);

        return false;
    }
}

bool Central::FnSendDeviceStatusUpdate(const std::string& deviceIP, const std::string& ec)
{
    const std::string uri_link = "http://" + centralServerIp;
    bool ret = false;

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        
        Poco::Net::HTTPClientSession session(uri.getHost(), centralServerPort);
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/DeviceStatus", Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        int retry = 0;
        
        if (!doSendDeviceStatusUpdate(session, request, response, deviceIP, ec))
        {
            retry = 3;

            while (retry > 0)
            {
                if (doSendDeviceStatusUpdate(session, request, response, deviceIP, ec))
                {
                    ret = true;
                    break;
                }
                else
                {
                    retry--;
                }
            }
        }
        else
        {
            ret = true;
        }
    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        ret = false;
    }
    
    return ret;
}

bool Central::doSendParkInParkOutInfo(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    AppLogger::getInstance()->FnLog(request.getURI());

    request.setContentType("application/json");

    Poco::JSON::Object::Ptr jsonObject = new Poco::JSON::Object;
    jsonObject->set("username", username);
    jsonObject->set("password", password);
    jsonObject->set("carpark_code", Iniparser::getInstance()->FnGetParkingLotLocationCode());
    // Temp: Need to revisit
    jsonObject->set("lot_no", "20");
    jsonObject->set("lpn", "SBC1929C");
    jsonObject->set("lot_in_image", "dGVzdGluZ2ZvcmltYWdlc3RyaW5n");
    jsonObject->set("lot_out_image", "dGVzdGluZ2ZvcmltYWdlc3RyaW5n");
    jsonObject->set("lot_in_time", "2023-12-31 00:00:00");
    jsonObject->set("lot_out_time", "2023-12-31 00:00:00");

    std::ostringstream jsonStringStream;
    jsonObject->stringify(jsonStringStream);
    request.setContentLength(jsonStringStream.str().length());

    // Send the request
    std::ostream& requestStream = session.sendRequest(request);
    requestStream << jsonStringStream.str();

    // Receive the response
    std::istream& responseStream = session.receiveResponse(response);

    // Log the response header
    std::ostringstream msg;
    msg << "Status : " << response.getStatus() << " Reason : " << response.getReason();
    AppLogger::getInstance()->FnLog(msg.str());

    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
    {
        std::ostringstream rs;
        Poco::StreamCopier::copyStream(responseStream, rs);
        AppLogger::getInstance()->FnLog(rs.str());

        return true;
    }
    else
    {
        Poco::NullOutputStream null;
        Poco::StreamCopier::copyStream(responseStream, null);

        return false;
    }
}

bool Central::FnSendParkInParkOutInfo()
{
    const std::string uri_link = "http://" + centralServerIp;
    bool ret = false;

    try
    {
        Poco::URI uri(uri_link);
        std::string path(uri.getPathAndQuery());
        if (path.empty())
        {
            path = "/";
        }
        
        Poco::Net::HTTPClientSession session(uri.getHost(), centralServerPort);
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, "/ParkInOut", Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;

        int retry = 0;
        
        if (!doSendParkInParkOutInfo(session, request, response))
        {
            retry = 3;

            while (retry > 0)
            {
                if (doSendParkInParkOutInfo(session, request, response))
                {
                    ret = true;
                    break;
                }
                else
                {
                    retry--;
                }
            }
        }
        else
        {
            ret = true;
        }

    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        ret = false;
    }
    
    return ret;
}
