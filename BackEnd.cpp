#include "BackEnd.h"

BackEnd::BackEnd(QObject *parent) : QObject(parent) {
    //create the database and populate the tables
    //set the private variables
    //if this is set to TRUE, tables will be dropped!
    createDatabase(true);
}
//getter methods
QString BackEnd::connectionString() {
    return serverString;
}

int BackEnd::connectionStringStatus() {
    return stringStatus;
}

bool BackEnd::sendGpsDataStatus() {
    //will return the status of the sent GPS data
    return sendFlag;
}

QString BackEnd::gpsLocation() {
    return "Timestamp: " + timestamp + "\n" + "Coordinates: " + coordinates;
}

//setter methods
void BackEnd::setConnectionString(const QString &serverString) {
    if(serverString != "") {
        this->serverString = serverString;

        updateServerString();
    }
}

void BackEnd::setGpsLocation(QGeoPositionInfo position) {
    if(position.isValid()){
        this->timestamp = timestamp;
        this->coordinates = coordinates;

        sendGpsData();
    }
}

//private methods
void BackEnd::updateServerString() {
    //perform a server test with the new string
    //TODO::
    stringStatus = 1;

    //before adding the new table entry, drop the table
    //only one server string allowed per device
    sqlQuery =
        "DELETE FROM Server;";

    if(!query.exec(sqlQuery)) {
        qInfo() << "[ERROR] DELETING From Table 'Server'";
        qInfo() << query.lastError();
    }

    query.finish();

    sqlQuery =
            "INSERT INTO Server (server_key, valid_flag) "
            "VALUES (:string, :flag);";

    query.prepare(sqlQuery);
    query.bindValue(":string", serverString);
    query.bindValue(":flag", stringStatus);

    if(!query.exec()) {
        qInfo() << "[ERROR] Inserting Into Table 'Server'";
        qInfo() << query.lastError();
    }

    query.finish();

    emit connectionStringChanged();
}

void BackEnd::sendGpsData() {
    //Test to see if theres an internet connection and the server is available
    //TODO::
    bool connectionFlag = false;

    if(stringStatus == 1 && connectionFlag) {
        //there is an internet connection
        //server string is valid
        //send ALL stored database entries, then delete the entries
        sendFlag = true;

        emit gpsLocationDataSent();
    }
    else {
        //no internet connection
        //server string is invalid
        //store the coordinates in the database
        sqlQuery =
                "INSERT INTO Locations (coordinate, timestamp) "
                "VALUES (:coordinate, :timestamp);";

        query.prepare(sqlQuery);
        query.bindValue(":coordinate", coordinates);
        query.bindValue(":timestamp", timestamp);

        if(!query.exec()) {
            qInfo() << "[ERROR] Inserting Into Table 'Locations'";
            qInfo() << query.lastError();
        }

        query.finish();

        sendFlag = false;

        emit gpsLocationDataSent();
    }
}

//create / load the database
void BackEnd::createDatabase(bool dropTablesFlag) {
    //create a new database or load it if one already exists
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data.db");
    db.open();

    query = QSqlQuery(db);

    //////////////////////////////////////////////////////////////
    //[WARNING] if dropTablesFlag == TRUE TABLES WILL BE DROPPED!!
    //////////////////////////////////////////////////////////////
    if(dropTablesFlag) {
        sqlQuery =
            "DROP TABLE IF EXISTS Server;";

        if(!query.exec(sqlQuery)) {
            qInfo() << "[ERROR] DROPPING Table 'Server'";
            qInfo() << query.lastError();
        }
        else {
            qInfo() << "[SUCCESS] DROPPED TABLE 'Server'";
        }
        query.finish();

        sqlQuery =
            "DROP TABLE IF EXISTS Locations;";

        if(!query.exec(sqlQuery)) {
            qInfo() << "[ERROR] DROPPING Table 'Locations'";
            qInfo() << query.lastError();
        }
        else {
            qInfo() << "[SUCCESS] DROPPED TABLE 'Locations'";
        }
        query.finish();
    }
    //////////////////////////////////////////////////////////////

    //create the database tables, will fail if they already exist
    sqlQuery =
        "CREATE TABLE Server ( "
            "server_key TEXT NOT NULL, "
            "valid_flag INTEGER NOT NULL "
        ");";

    if(!query.exec(sqlQuery)) {
        qInfo() << "[ERROR] Creating Table 'Server'";
        qInfo() << query.lastError();
    }
    else {
        //table is being created for the first time
        //add dummy value to show tutorial
        query.finish();

        sqlQuery =
            "INSERT INTO Server (server_key, valid_flag) "
            "VALUES (' ', -1) "
            ";";

        if(!query.exec(sqlQuery)) {
            qInfo() << "[ERROR] Inserting Into Table 'Server'";
            qInfo() << query.lastError();
        }
    }

    query.finish();

    sqlQuery =
        "CREATE TABLE Locations ( "
            "index_entry INTEGER NOT NULL AUTOINCREMENT, "
            "coordinate TEXT NOT NULL, "
            "timestamp TEXT NOT NULL, "
            "PRIMARY KEY(index_entry) "
        ");";

    if(!query.exec(sqlQuery)) {
        qInfo() << "[ERROR] Creating Table 'Locations'";
        qInfo() << query.lastError();
    }

    query.finish();

    //retrieve the server key from the database
    sqlQuery =
        "SELECT "
            "server_key, "
            "valid_flag "
        "FROM "
            "Server "
        ";";

    if(!query.exec(sqlQuery)) {
        qInfo() << "[ERROR] Selecting From Table 'Server'";
        qInfo() << query.lastError();
    }

    query.first();

    serverString  = query.value(0).toString();
    stringStatus = query.value(1).toInt();

    query.finish();

    qInfo() << "serverString: " + serverString;
    qInfo() << "stringStatus: " + QString::number(stringStatus);
}
