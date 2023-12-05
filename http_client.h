#include <iostream>

class httpClient
{

public:
    static httpClient* getInstance();

    httpClient(httpClient& httpclient) = delete;

    void operator=(const httpClient&) = delete;

private:
    static httpClient* httpClient_;
    httpClient();
};
