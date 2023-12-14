#include <iostream>
#include <string.h>
#include <sstream>
#include "database.h"
#include "ini_parser.h"
#include "camera.h"
#include "central.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPDigestCredentials.h"
#include "Poco/Path.h"
#include "Poco/StreamCopier.h"
#include "Poco/StringTokenizer.h"
#include "Poco/FileStream.h"
#include "log.h"

using namespace Poco::Net;
using namespace Poco;


int main(int argc, char* agrv[])
{
    std::ostringstream info;
    info << "start " << agrv[0] << " , version: 0.0.1 build:" << __DATE__ << " " << __TIME__;
    AppLogger::getInstance()->FnLog(info.str());

    //std::cout<< "Result:" << Camera::getInstance()->FnGetHeartBeat() << std::endl;
    //std::cout<< "Result:" << Camera::getInstance()->FnGetSnapShot() << std::endl;
    //std::cout<< "Result:" << Camera::getInstance()->FnSubscibeToSnapShotParked() << std::endl;
    //Camera::getInstance()->FnSubscribeToSnapShot();
    
    //Camera::getInstance()->FnSetCurrentTime();
    //std::string time;
    //Camera::getInstance()->FnGetCurrentTime(time);
    //std::cout << time << std::endl;

    /*
    Database::getInstance()->FnDatabaseInit();
    Database::parking_lot_t lot = 
    {"1", 
     "2",
     "ABC 1234",
     "/usr/local/image/in",
     "/usr/local/image/out",
     "2023-12-13 01:59:59",
     "2023-12-13 05:59:59",
     "",
     "2023-12-13 14:59:59",
     "2023-12-13 20:59:59",
     "2023-12-13 23:59:59"
     };
    Database::getInstance()->FnInsertRecord("tbl_ev_lot_trans", lot);
    std::vector<Database::parking_lot_t> lots;
    Database::getInstance()->FnSelectAllRecord("tbl_ev_lot_trans", lots);

    for (auto& parking_lot : lots)
    {
        std::cout << "parking_lot.location_code : " << parking_lot.location_code << std::endl;
        std::cout << "parking_lot.lot_no : " << parking_lot.lot_no << std::endl;
        std::cout << "parking_lot.lpn : " << parking_lot.lpn << std::endl;
        std::cout << "parking_lot.lot_in_image : " << parking_lot.lot_in_image_path << std::endl;
        std::cout << "parking_lot.lot_out_image : " << parking_lot.lot_out_image_path << std::endl;
        std::cout << "parking_lot.lot_in_dt : " << parking_lot.lot_in_dt << std::endl;
        std::cout << "parking_lot.lot_out_dt : " << parking_lot.lot_out_dt << std::endl;
        std::cout << "parking_lot.add_dt : " << parking_lot.add_dt << std::endl;
        std::cout << "parking_lot.update_dt : " << parking_lot.update_dt << std::endl;
        std::cout << "parking_lot.lot_in_central_sent_dt : " << parking_lot.lot_in_central_sent_dt << std::endl;
        std::cout << "parking_lot.lot_out_central_sent_dt : " << parking_lot.lot_out_central_sent_dt << std::endl;
    }

    Database::getInstance()->FnRemoveAllRecord("tbl_ev_lot_trans");
    */

    //Iniparser::getInstance()->FnIniParserInit();
    //std::cout << Iniparser::getInstance()->FnGetParkingLotLocationCode() << std::endl;
    //std::cout << Iniparser::getInstance()->FnGetTimerForFilteringSnapShot() << std::endl;

    std::cout << "Result : " << Central::getInstance()->FnSendHeartBeatUpdate() << std::endl;
    std::cout << "Result : " << Central::getInstance()->FnSendDeviceStatusUpdate() << std::endl;
    std::cout << "Result : " << Central::getInstance()->FnSendParkInParkOutInfo() << std::endl;

    return 0;
}