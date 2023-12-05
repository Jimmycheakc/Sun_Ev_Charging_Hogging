#include "http_client.h"

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
