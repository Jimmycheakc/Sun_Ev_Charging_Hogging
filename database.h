#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "Poco/Data/Session.h"
#include "Poco/Mutex.h"

class Database{

public:
    typedef struct parking_lot
    {
        std::string location_code;
        std::string lot_no;
        std::string lpn;
        std::string lot_in_image_path;
        std::string lot_out_image_path;
        std::string lot_in_dt;
        std::string lot_out_dt;
        std::string add_dt;
        std::string update_dt;
        std::string lot_in_central_sent_dt;
        std::string lot_out_central_sent_dt;
    } parking_lot_t;

    static Database* getInstance();
    void FnDatabaseInit();
    void FnInsertRecord(const std::string& tableName, parking_lot_t lot);
    void FnSelectAllRecord(const std::string& tableName, std::vector<parking_lot_t>& v_lot);
    bool FnIsTableEmpty(const std::string& tableName);
    void FnRemoveAllRecord(const std::string& tableName);

    void FnUpdateThreeLotParkingStatus(const std::string& tableName);
    const parking_lot_t& FnGetFirstParkingLot() const;
    const parking_lot_t& FnGetSecondParkingLot() const;
    const parking_lot_t& FnGetThirdParkingLot() const;

    Database(Database& database) = delete;

    void operator=(const Database&) = delete;

private:
    static Database* database_;
    std::unique_ptr<Poco::Data::Session> session_;
    Poco::Mutex databaseMutex_;
    parking_lot_t first_parking_lot;
    parking_lot_t second_parking_lot;
    parking_lot_t third_parking_lot;
    Database();
    ~Database();
};