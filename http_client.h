#include <iostream>
#include <string>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"

class httpClient
{
public:
    const std::string username = "admin";
    const std::string password = "nexpa1234";

    static httpClient* getInstance();
    bool FnGetHeartBeat();

    httpClient(httpClient& httpclient) = delete;

    void operator=(const httpClient&) = delete;

private:
    static httpClient* httpClient_;
    httpClient();
    std::istream& do_request(const std::string& uri_link);
    bool do_request(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& reqeust, Poco::Net::HTTPResponse& response);
};
