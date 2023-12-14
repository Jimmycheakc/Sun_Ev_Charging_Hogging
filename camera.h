#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/File.h"
#include "Poco/Path.h"

class Camera
{
public:
    typedef struct event{
        std::string evt_channel;
        std::string evt_parking_status;
        std::string evt_lane;
        std::string evt_lpn;
        std::string evt_snapshot_time;
    } event_t;
    
    const std::string username = "admin";
    const std::string password = "nexpa1234";
    const std::string cameraServerIP = "192.168.2.166";
    const std::string imageDirectoryPath = Poco::Path::home() + "Desktop/Ev_Charging_Hogging_Image";

    static Camera* getInstance();
    bool FnGetHeartBeat();
    bool FnGetSnapShot();
    bool FnSubscibeToSnapShotParked();
    bool FnSubscribeToSnapShot();
    bool FnSetCurrentTime();
    bool FnGetCurrentTime(std::string& dateTime);

    Camera(Camera& camera) = delete;

    void operator=(const Camera&) = delete;

private:
    static Camera* camera_;
    Camera();
    void createImageDirectory();
    bool isImageDirectoryExists();
    bool do_heartBeatRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response);
    bool do_snapShotRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response);
    bool do_subscribeToSnapShotParked(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response);
    bool do_subscribeToSnapShot(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response);
    bool do_setCurrentTime(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response);
    bool do_getCurrentTime(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response, std::string& dateTime);
};
