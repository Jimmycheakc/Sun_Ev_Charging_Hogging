#include <iostream>
#include <string>
#include <sstream>
#include "http_client.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPDigestCredentials.h"
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "log.h"

httpClient* httpClient::httpClient_ = nullptr;

httpClient::httpClient()
{
}

httpClient* httpClient::getInstance()
{
    if (httpClient_ == nullptr)
    {
        httpClient_ = new httpClient();
    }

    return httpClient_;
}

std::istream& httpClient::do_request(const std::string& uri_link)
{
    Poco::URI uri(uri_link);
    Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);
    session.sendRequest(request);

    // Get Unauthorized Response
    Poco::Net::HTTPResponse response;
    session.receiveResponse(response);

    // Request with Authentication
    Poco::Net::HTTPDigestCredentials credentials(username, password);
    credentials.authenticate(request, response);
    session.sendRequest(request);

    // Get the Response
    return session.receiveResponse(response);
}

bool httpClient::do_request(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
    session.sendRequest(request);
    std::istream& rs = session.receiveResponse(response);
    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
    {
        std::ostringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);
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

bool httpClient::FnGetHeartBeat()
{
    const std::string uri_link= "http://192.168.2.166/cgi-bin/trafficParking.cgi?action=getAllParkingSpaceStatus";

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

        if (!do_request(session, request, response))
        {
            credentials.authenticate(request, response);
            if (!do_request(session, request, response))
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
