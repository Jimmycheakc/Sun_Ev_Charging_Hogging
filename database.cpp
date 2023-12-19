#include <iostream>
#include <string>
#include <sstream>
#include "database.h"
#include "log.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Data/Session.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/RecordSet.h"
#include "Poco/Mutex.h"

Database* Database::database_ = nullptr;

Database::Database()
    : first_parking_lot{},
    second_parking_lot{},
    third_parking_lot{}
{
}

Database::~Database()
{
    Poco::Data::MySQL::Connector::unregisterConnector();
}

Database* Database::getInstance()
{
    if (database_ == nullptr)
    {
        database_ = new Database();
    }
    return database_;
}

void Database::FnDatabaseInit()
{
    try
    {
        // Initialize MySQL connector
        Poco::Data::MySQL::Connector::registerConnector();

        // Establish a session
        session_ = std::make_unique<Poco::Data::Session>("MySQL", "host=localhost;user=root;password=yzxh2007;db=ev_charging_hogging_database");
    }
    catch (const Poco::Exception& ex)
    {
        std::ostringstream msg;
        msg << "POCO Exception: " << ex.displayText() << std::endl;
        std::cerr << msg.str();
        AppLogger::getInstance()->FnLog(msg.str());
    }
}

void Database::FnInsertRecord(const std::string& tableName, parking_lot_t lot)
{
    // Local scope lock
    Poco::Mutex::ScopedLock lock(databaseMutex_);

    if (!session_->isConnected())
    {
        session_->reconnect();
    }

    try
    {
        std::string query = "INSERT INTO " + tableName + " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        Poco::Data::Statement insert(*session_);

        Poco::Nullable<std::string> location_code(lot.location_code.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.location_code));
        Poco::Nullable<std::string> lot_no(lot.lot_no.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_no));
        Poco::Nullable<std::string> lpn(lot.lpn.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lpn));
        Poco::Nullable<std::string> lot_in_image_path(lot.lot_in_image_path.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_in_image_path));
        Poco::Nullable<std::string> lot_out_image_path(lot.lot_out_image_path.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_out_image_path));
        Poco::Nullable<std::string> lot_in_dt(lot.lot_in_dt.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_in_dt));
        Poco::Nullable<std::string> lot_out_dt(lot.lot_out_dt.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_out_dt));
        Poco::Nullable<std::string> add_dt(lot.add_dt.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.add_dt));
        Poco::Nullable<std::string> update_dt(lot.update_dt.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.update_dt));
        Poco::Nullable<std::string> lot_in_central_sent_dt(lot.lot_in_central_sent_dt.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_in_central_sent_dt));
        Poco::Nullable<std::string> lot_out_central_sent_dt(lot.lot_out_central_sent_dt.empty() ? Poco::Nullable<std::string>() : Poco::Nullable<std::string>(lot.lot_out_central_sent_dt));

        insert << query,
                Poco::Data::Keywords::use(location_code),
                Poco::Data::Keywords::use(lot_no),
                Poco::Data::Keywords::use(lpn),
                Poco::Data::Keywords::use(lot_in_image_path),
                Poco::Data::Keywords::use(lot_out_image_path),
                Poco::Data::Keywords::use(lot_in_dt),
                Poco::Data::Keywords::use(lot_out_dt),
                Poco::Data::Keywords::use(add_dt),
                Poco::Data::Keywords::use(update_dt),
                Poco::Data::Keywords::use(lot_in_central_sent_dt),
                Poco::Data::Keywords::use(lot_out_central_sent_dt);

        insert.execute();
    }
    catch(const Poco::Exception& ex)
    {
        std::ostringstream msg;
        msg << __func__ << " POCO Exception: " << ex.displayText() << std::endl;
        std::cerr << msg.str();
        AppLogger::getInstance()->FnLog(msg.str());
    }
    
}

void Database::FnSelectAllRecord(const std::string& tableName, std::vector<parking_lot_t>& v_lot)
{
    // Local scope lock
    Poco::Mutex::ScopedLock lock(databaseMutex_);

    if (!session_->isConnected())
    {
        session_->reconnect();
    }

    try
    {
        std::string query = "SELECT * FROM " + tableName;
        Poco::Data::Statement select(*session_);
        select << query;

        select.execute();

        Poco::Data::RecordSet recordSet(select);
        if (recordSet.moveFirst())
        {
            do
            {
                parking_lot_t lot;
                memset(&lot, 0, sizeof(parking_lot_t));

                lot.location_code = recordSet["location_code"].isEmpty() ? "NULL" : recordSet["location_code"].convert<std::string>();
                lot.lot_no = recordSet["lot_no"].isEmpty() ? "NULL" : recordSet["lot_no"].convert<std::string>();
                lot.lpn = recordSet["lpn"].isEmpty() ? "NULL" : recordSet["lpn"].convert<std::string>();
                lot.lot_in_image_path = recordSet["lot_in_image"].isEmpty() ? "NULL" : recordSet["lot_in_image"].convert<std::string>();
                lot.lot_out_image_path = recordSet["lot_out_image"].isEmpty() ? "NULL" : recordSet["lot_out_image"].convert<std::string>();
                lot.lot_in_dt = recordSet["lot_in_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSet["lot_in_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
                lot.lot_out_dt = recordSet["lot_out_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSet["lot_out_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
                lot.add_dt = recordSet["add_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSet["add_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
                lot.update_dt = recordSet["update_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSet["update_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
                lot.lot_in_central_sent_dt = recordSet["lot_in_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSet["lot_in_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
                lot.lot_out_central_sent_dt = recordSet["lot_out_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSet["lot_out_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");

                v_lot.push_back(lot);
            } while (recordSet.moveNext());
        }

    }
    catch(const Poco::Exception& ex)
    {
        std::ostringstream msg;
        msg << __func__ << " POCO Exception: " << ex.displayText() << std::endl;
        std::cerr << msg.str();
        AppLogger::getInstance()->FnLog(msg.str());
    }
}

bool Database::FnIsTableEmpty(const std::string& tableName)
{
    Poco::Mutex::ScopedLock lock(databaseMutex_);

    if (!session_->isConnected())
    {
        session_->reconnect();
    }

    try
    {
        std::string query = "SELECT COUNT(*) FROM " + tableName;
        Poco::Data::Statement select(*session_);
        select << query;

        select.execute();

        Poco::Data::RecordSet recordSet(select);
        
        if (recordSet.moveFirst())
        {
            int count;
            count = recordSet["COUNT(*)"].convert<int>();
            return count == 0;
        }

        return false;
    }
    catch(const Poco::Exception& ex)
    {
        std::ostringstream msg;
        msg << __func__ << " POCO Exception: " << ex.displayText() << std::endl;
        std::cerr << msg.str();
        AppLogger::getInstance()->FnLog(msg.str());

        return false;
    }
    
}

void Database::FnRemoveAllRecord(const std::string& tableName)
{
    // Local scope lock
    Poco::Mutex::ScopedLock lock(databaseMutex_);

    if (!session_->isConnected())
    {
        session_->reconnect();
    }

    try
    {
        std::string query = "DELETE FROM " + tableName;
        Poco::Data::Statement remove(*session_);
        remove << query;

        remove.execute();
    }
    catch(const Poco::Exception& ex)
    {
        std::ostringstream msg;
        msg << __func__ << " POCO Exception: " << ex.displayText() << std::endl;
        std::cerr << msg.str();
        AppLogger::getInstance()->FnLog(msg.str());
    }
}

void Database::FnUpdateThreeLotParkingStatus(const std::string& tableName)
{
    // Local scope lock
    Poco::Mutex::ScopedLock lock(databaseMutex_);

    if (!session_->isConnected())
    {
        session_->reconnect();
    }

    try
    {
        // Query on the lot no == '1' and latest time in the record
        std::string query_first_lot = "SELECT * FROM " + tableName + " WHERE lot_no = '1' ORDER BY COALESCE(lot_in_dt, lot_out_dt) DESC LIMIT 1";
        Poco::Data::Statement select_first_lot(*session_);
        select_first_lot << query_first_lot;

        select_first_lot.execute();
        Poco::Data::RecordSet recordSetFirstLot(select_first_lot);
        if (recordSetFirstLot.moveFirst())
        {
            first_parking_lot.location_code = recordSetFirstLot["location_code"].isEmpty() ? "NULL" : recordSetFirstLot["location_code"].convert<std::string>();
            first_parking_lot.lot_no = recordSetFirstLot["lot_no"].isEmpty() ? "NULL" : recordSetFirstLot["lot_no"].convert<std::string>();
            first_parking_lot.lpn = recordSetFirstLot["lpn"].isEmpty() ? "NULL" : recordSetFirstLot["lpn"].convert<std::string>();
            first_parking_lot.lot_in_image_path = recordSetFirstLot["lot_in_image"].isEmpty() ? "NULL" : recordSetFirstLot["lot_in_image"].convert<std::string>();
            first_parking_lot.lot_out_image_path = recordSetFirstLot["lot_out_image"].isEmpty() ? "NULL" : recordSetFirstLot["lot_out_image"].convert<std::string>();
            first_parking_lot.lot_in_dt = recordSetFirstLot["lot_in_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetFirstLot["lot_in_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            first_parking_lot.lot_out_dt = recordSetFirstLot["lot_out_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetFirstLot["lot_out_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            first_parking_lot.add_dt = recordSetFirstLot["add_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetFirstLot["add_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            first_parking_lot.update_dt = recordSetFirstLot["update_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetFirstLot["update_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            first_parking_lot.lot_in_central_sent_dt = recordSetFirstLot["lot_in_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetFirstLot["lot_in_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            first_parking_lot.lot_out_central_sent_dt = recordSetFirstLot["lot_out_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetFirstLot["lot_out_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
        }

        // Query on the lot no == '2' and latest time in the record
        std::string query_second_lot = "SELECT * FROM " + tableName + " WHERE lot_no = '2' ORDER BY COALESCE(lot_in_dt, lot_out_dt) DESC LIMIT 1";
        Poco::Data::Statement select_second_lot(*session_);
        select_second_lot << query_second_lot;

        select_second_lot.execute();
        Poco::Data::RecordSet recordSetSecondLot(select_second_lot);
        if (recordSetSecondLot.moveFirst())
        {
            second_parking_lot.location_code = recordSetSecondLot["location_code"].isEmpty() ? "NULL" : recordSetSecondLot["location_code"].convert<std::string>();
            second_parking_lot.lot_no = recordSetSecondLot["lot_no"].isEmpty() ? "NULL" : recordSetSecondLot["lot_no"].convert<std::string>();
            second_parking_lot.lpn = recordSetSecondLot["lpn"].isEmpty() ? "NULL" : recordSetSecondLot["lpn"].convert<std::string>();
            second_parking_lot.lot_in_image_path = recordSetSecondLot["lot_in_image"].isEmpty() ? "NULL" : recordSetSecondLot["lot_in_image"].convert<std::string>();
            second_parking_lot.lot_out_image_path = recordSetSecondLot["lot_out_image"].isEmpty() ? "NULL" : recordSetSecondLot["lot_out_image"].convert<std::string>();
            second_parking_lot.lot_in_dt = recordSetSecondLot["lot_in_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetSecondLot["lot_in_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            second_parking_lot.lot_out_dt = recordSetSecondLot["lot_out_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetSecondLot["lot_out_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            second_parking_lot.add_dt = recordSetSecondLot["add_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetSecondLot["add_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            second_parking_lot.update_dt = recordSetSecondLot["update_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetSecondLot["update_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            second_parking_lot.lot_in_central_sent_dt = recordSetSecondLot["lot_in_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetSecondLot["lot_in_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            second_parking_lot.lot_out_central_sent_dt = recordSetSecondLot["lot_out_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetSecondLot["lot_out_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
        }

        // Query on the lot no == '3' and latest time in the record
        std::string query_third_lot = "SELECT * FROM " + tableName + " WHERE lot_no = '3' ORDER BY COALESCE(lot_in_dt, lot_out_dt) DESC LIMIT 1";
        Poco::Data::Statement select_third_lot(*session_);
        select_third_lot << query_third_lot;

        select_third_lot.execute();
        Poco::Data::RecordSet recordSetThirdLot(select_third_lot);
        if (recordSetThirdLot.moveFirst())
        {
            third_parking_lot.location_code = recordSetThirdLot["location_code"].isEmpty() ? "NULL" : recordSetThirdLot["location_code"].convert<std::string>();
            third_parking_lot.lot_no = recordSetThirdLot["lot_no"].isEmpty() ? "NULL" : recordSetThirdLot["lot_no"].convert<std::string>();
            third_parking_lot.lpn = recordSetThirdLot["lpn"].isEmpty() ? "NULL" : recordSetThirdLot["lpn"].convert<std::string>();
            third_parking_lot.lot_in_image_path = recordSetThirdLot["lot_in_image"].isEmpty() ? "NULL" : recordSetThirdLot["lot_in_image"].convert<std::string>();
            third_parking_lot.lot_out_image_path = recordSetThirdLot["lot_out_image"].isEmpty() ? "NULL" : recordSetThirdLot["lot_out_image"].convert<std::string>();
            third_parking_lot.lot_in_dt = recordSetThirdLot["lot_in_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetThirdLot["lot_in_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            third_parking_lot.lot_out_dt = recordSetThirdLot["lot_out_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetThirdLot["lot_out_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            third_parking_lot.add_dt = recordSetThirdLot["add_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetThirdLot["add_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            third_parking_lot.update_dt = recordSetThirdLot["update_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetThirdLot["update_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            third_parking_lot.lot_in_central_sent_dt = recordSetThirdLot["lot_in_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetThirdLot["lot_in_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
            third_parking_lot.lot_out_central_sent_dt = recordSetThirdLot["lot_out_central_sent_dt"].isEmpty() ? "NULL" : Poco::DateTimeFormatter::format(recordSetThirdLot["lot_out_central_sent_dt"].convert<Poco::DateTime>(), "%Y-%m-%d %H:%M:%S");
        }
    }
    catch(const Poco::Exception& ex)
    {
        std::ostringstream msg;
        msg << __func__ << " POCO Exception: " << ex.displayText() << std::endl;
        std::cerr << msg.str();
        AppLogger::getInstance()->FnLog(msg.str());
    }
}

const Database::parking_lot_t& Database::FnGetFirstParkingLot() const
{
    return first_parking_lot;
}

const Database::parking_lot_t& Database::FnGetSecondParkingLot() const
{
    return second_parking_lot;
}

const Database::parking_lot_t& Database::FnGetThirdParkingLot() const
{
    return third_parking_lot;
}