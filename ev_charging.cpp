#include <iostream>
#include <string.h>
#include "http_client.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPDigestCredentials.h"
#include "Poco/Path.h"
#include "Poco/StreamCopier.h"
#include "Poco/StringTokenizer.h"

using namespace Poco::Net;
using namespace Poco;

int main(int argc, char* agrv[])
{
    const std::string uri_link= "http://192.168.2.166/cgi-bin/magicBox.cgi?action=getLanguageCaps";

    URI uri(uri_link);
    std::cout << "host: " << uri.getHost() << " Port: " << uri.getPort() << std::endl;

    HTTPClientSession session(uri.getHost(), uri.getPort());
    std::string path(uri.getPathAndQuery());

    std::cout << path << std::endl;

    // Send request
    HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    req.add("User-Agent", "client/1.0");
    req.setContentLength(0);
    session.sendRequest(req);

    // Get response
    HTTPResponse res;
    std::istream &is = session.receiveResponse(res);
    std::cout << res.getStatus() << " " << res.getReason() << std::endl;

    std::cout << "Response Header : " << std::endl;
    res.write(std::cout);
    const std::string wwwAuthenticateHeader = res.get("WWW-Authenticate");
    StringTokenizer tokenizer(wwwAuthenticateHeader, ",\"", StringTokenizer::TOK_TRIM | StringTokenizer::TOK_IGNORE_EMPTY);

    std::cout << "Tokens from StringTokenizer:" << std::endl;
    if (tokenizer[0].find("realm="))
    {
        tokenizer.replace(tokenizer[0], "realm=");
    }
    for (const auto& token : tokenizer) {
        std::cout << token << std::endl;
    }

    std::string realm, nonce, opaque, qop;
    for (int i = 0; i < tokenizer.count(); i++)
    {
        std::string token = tokenizer[i];
        if (icompare(token, "realm=") == 0)
        {
            realm = tokenizer[i + 1];
        }
        else if (icompare(token, "qop=") == 0)
        {
            qop = tokenizer[i + 1];
        }
        else if (icompare(token, "nonce=") == 0)
        {
            nonce = tokenizer[i + 1];
        }
        else if (icompare(token, "opaque=") == 0)
        {
            opaque = tokenizer[i + 1];
        }
    }

    std::cout << "realm : " << realm << std::endl;
    std::cout << "qop : " << qop << std::endl;
    std::cout << "nonce : " << nonce << std::endl;
    std::cout << "opaque : " << opaque << std::endl;

    std::string nc = "00000001";
    std::string cnonce = "0a4f113b";

    HTTPDigestCredentials credentials("admin", "nexpa1234");
    credentials.authenticate(req, res);

    session.sendRequest(req);

    // Get the new response
    //session.receiveResponse(res);

    // Print the new response
    std::istream &responseStream = session.receiveResponse(res);
    StreamCopier::copyStream(responseStream, std::cout);
    std::cout << res.getStatus() << " " << res.getReason() << std::endl;

    std::cout << "Response Header : " << std::endl;
    res.write(std::cout);

    
    return 0;
}
