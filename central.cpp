#include <iostream>
#include <string>
#include <sstream>
#include "central.h"
#include "log.h"
#include "Poco/Exception.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"

Central* Central::central_ = nullptr;

Central::Central()
{
}

Central* Central::getInstance()
{
    if (central_ == nullptr)
    {
        central_ = new Central();
    }
    return central_;
}

bool Central::FnSendHeartBeatUpdate()
{
    const std::string uri_link = "http://" + centralServerIp;

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
        request.setContentType("application/json");

        std::string jsonData = R"({
            "username": "testuser",
            "password": "testpassword",
            "carpark_code": "PL80",
            "heartbeat_dt": "2023-11-30 00:00:00",
            "msg": "Hearbeat Update"
        })";
        request.setContentLength(jsonData.length());

        // Send the request
        std::ostream& rs = session.sendRequest(request);
        rs << jsonData;

        // Receive the response
        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse(response);

        std::cout << "Status : " << response.getStatus() << " Reason : " << response.getReason() << std::endl;

        /// Process the response
        std::string responseData;
        Poco::StreamCopier::copyToString(responseStream, responseData);
        std::cout << "Response: " << responseData << std::endl;

    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }
    
    return true;
}

bool Central::FnSendDeviceStatusUpdate()
{
    const std::string uri_link = "http://" + centralServerIp;

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
        request.setContentType("application/json");

        std::string jsonData = R"({
            "username": "testuser",
            "password": "testpassword",
            "carpark_code": "PL80",
            "device_ip": "192.168.2.120",
            "error_code": "1"
        })";
        request.setContentLength(jsonData.length());

        // Send the request
        std::ostream& rs = session.sendRequest(request);
        rs << jsonData;

        // Receive the response
        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse(response);

        std::cout << "Status : " << response.getStatus() << " Reason : " << response.getReason() << std::endl;

        /// Process the response
        std::string responseData;
        Poco::StreamCopier::copyToString(responseStream, responseData);
        std::cout << "Response: " << responseData << std::endl;

    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }
    
    return true;
}

bool Central::FnSendParkInParkOutInfo()
{
    const std::string uri_link = "http://" + centralServerIp;

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
        request.setContentType("application/json");

        std::string jsonData = R"({
            "username": "testuser",
            "password": "testpassword",
            "carpark_code": "PL80",
            "lot_no": "20",
            "lpn": "SBC1929C",
            "lot_in_image": "dGVzdGluZ2ZvcmltYWdlc3RyaW5n",
            "lot_out_image": "dGVzdGluZ2ZvcmltYWdlc3RyaW5n",
            "lot_in_time": "2023-12-31 00:00:00",
            "lot_out_time": "2023-12-31 00:00:00"
        })";
        request.setContentLength(jsonData.length());

        // Send the request
        std::ostream& rs = session.sendRequest(request);
        rs << jsonData;

        // Receive the response
        Poco::Net::HTTPResponse response;
        std::istream& responseStream = session.receiveResponse(response);

        std::cout << "Status : " << response.getStatus() << " Reason : " << response.getReason() << std::endl;

        /// Process the response
        std::string responseData;
        Poco::StreamCopier::copyToString(responseStream, responseData);
        std::cout << "Response: " << responseData << std::endl;

    }
    catch (Poco::Exception& ex)
    {
        AppLogger::getInstance()->FnLog(ex.displayText());
        return false;
    }
    
    return true;
}
