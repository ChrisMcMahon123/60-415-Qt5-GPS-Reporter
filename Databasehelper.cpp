#include "Databasehelper.h"

DatabaseHelper::DatabaseHelper() {
    //connect to existing database or create a new one
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("data.db");
    database.open();

    sqlQuery = QSqlQuery(database);

    //sending 'true' to the function will drop all tables
    //and recreate them, all data will be lost
    createDatabase(false);
}
//create / load the database
void DatabaseHelper::createDatabase(bool dropTablesFlag) {
    //////////////////////////////////////////////////////////////
    //[WARNING] All data will be lost if dropTablesFlag == TRUE //
    //////////////////////////////////////////////////////////////
    if(dropTablesFlag) {
        queryString =
            "DROP TABLE IF EXISTS Server;";

        if(sqlQuery.exec(queryString)) {
            qInfo() << "\n[SUCCESS] DROPPED TABLE 'Server'";
        }
        else {
            qInfo() << "\n[FAILURE] DROPPING Table 'Server'";
            qInfo() << sqlQuery.lastError();
        }
        sqlQuery.finish();

        queryString =
            "DROP TABLE IF EXISTS Locations;";

        if(sqlQuery.exec(queryString)) {
            qInfo() << "\n[SUCCESS] DROPPED TABLE 'Locations'";
        }
        else {
            qInfo() << "\n[FAILURE] DROPPING Table 'Locations'";
            qInfo() << sqlQuery.lastError();
        }

        sqlQuery.finish();
    }
    //////////////////////////////////////////////////////////////

    //create the database tables, will fail if they already exist
    queryString =
        "CREATE TABLE Server ( "
            "server_key TEXT NOT NULL, "
            "key_status INTEGER NOT NULL "
        ");";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SUCCESS] Creating Table 'Server'";

        //table is being created for the first time
        //add dummy value to show tutorial
        sqlQuery.finish();

        queryString =
            "INSERT INTO Server (server_key, key_status) "
            "VALUES (' ', -1) "
            ";";

        if(sqlQuery.exec(queryString)) {
            qInfo() << "\n[SUCCESS] Inserting Into Table 'Server'";
        }
        else {
            qInfo() << "\n[FAILURE] Inserting Into Table 'Server'";
            qInfo() << sqlQuery.lastError();
        }
    }
    else {
        qInfo() << "\n[FAILURE] Creating Table 'Server'";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();

    queryString =
        "CREATE TABLE Locations ( "
            "index_entry INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            "latitude TEXT NOT NULL, "
            "longitude TEXT NOT NULL, "
            "timestamp TEXT NOT NULL "
        ");";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SUCCESS] Creating Table 'Locations'";
    }
    else {
        qInfo() << "\n[FAILURE] Creating Table 'Locations'";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

QHash<QString, QString> DatabaseHelper::getServerKey() {
    //retrieve the server key from the database
    queryString =
        "SELECT "
            "server_key, "
            "key_status "
        "FROM "
            "Server "
        ";";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SUCCESS] Selecting From Table 'Server'";
    }
    else {
        qInfo() << "\n[FAILURE] Selecting From Table 'Server'";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.first();

    QHash<QString, QString> hashmap;

    hashmap.insert("server_key", sqlQuery.value(0).toString());
    hashmap.insert("key_status", sqlQuery.value(1).toString());

    sqlQuery.finish();

    return hashmap;
}

void DatabaseHelper::insertServerKey(QString serverString, int stringStatus) {
    //before adding the new table entry, drop the table
    //only one server string allowed per device
    queryString =
        "DELETE FROM Server;";

    if(sqlQuery.exec(queryString)) {
        qInfo() << "\n[SUCCESS] DELETING From Table 'Server'";
    }
    else {
        qInfo() << "\n[FAILURE] DELETING From Table 'Server'";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();

    queryString =
            "INSERT INTO Server (server_key, key_status) "
            "VALUES (:string, :status);";

    sqlQuery.prepare(queryString);
    sqlQuery.bindValue(":string", serverString);
    sqlQuery.bindValue(":status", stringStatus);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SUCCESS] Inserting Into Table 'Server'";
    }
    else {
        qInfo() << "\n[FAILURE] Inserting Into Table 'Server'";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::insertLocation(const QString &latitude, const QString &longitude, const QString &timestamp) {
    //store the coordinates in the database
    queryString =
            "INSERT INTO Locations (latitude, longitude, timestamp) "
            "VALUES (:latitude, :longitude, :timestamp);";

    sqlQuery.prepare(queryString);
    sqlQuery.bindValue(":latitude", latitude);
    sqlQuery.bindValue(":longitude", longitude);
    sqlQuery.bindValue(":timestamp", timestamp);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SUCCESS] Inserting Into Table 'Locations'";
    }
    else {
        qInfo() << "\n[FAILURE] Inserting Into Table 'Locations'";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}
