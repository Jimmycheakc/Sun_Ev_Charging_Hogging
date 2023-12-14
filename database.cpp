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

Database* Database::database_ = nullptr;

Database::Database()
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

void Database::FnRemoveAllRecord(const std::string& tableName)
{
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