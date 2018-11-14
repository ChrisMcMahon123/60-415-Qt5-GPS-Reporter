#include "Databasehelper.h"

DatabaseHelper::DatabaseHelper() {
    //default constructor
}

DatabaseHelper::DatabaseHelper(bool flag) {
    //connect to existing database or create a new one
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("data.db");
    database.open();

    sqlQuery = QSqlQuery(database);

    createDatabase(flag);
}

//create / load the database
void DatabaseHelper::createDatabase(bool dropTablesFlag) {
    //////////////////////////////////////////////////////////////
    //[WARNING] All data will be lost if dropTablesFlag == TRUE //
    //////////////////////////////////////////////////////////////
    if(dropTablesFlag) {
        if(sqlQuery.exec("DROP TABLE IF EXISTS Settings")) {
            qInfo() << "\n[SQL][SUCCESS][DROP][Settings]\n";
        }
        else {
            qInfo() << "\n[SQL][FAILURE][DROP][Settings]\n";
            qInfo() << sqlQuery.lastError();
        }

        sqlQuery.finish();

        if(sqlQuery.exec("DROP TABLE IF EXISTS Locations")) {
            qInfo() << "\n[SQL][SUCCESS][DROP][Locations]\n";
        }
        else {
            qInfo() << "\n[SQL][FAILURE][DROP][Locations]\n";
            qInfo() << sqlQuery.lastError();
        }

        sqlQuery.finish();
    }
    //////////////////////////////////////////////////////////////

    //create the database tables, will show error if they already exist (thats okay)
    if(sqlQuery.exec("CREATE TABLE Settings ( "
                        "access_code TEXT NOT NULL, "
                        "code_status INTEGER NOT NULL, "
                        "checkbox_value INTEGER NOT NULL, "
                        "json_webtoken TEXT NOT NULL "
                    ")")) {

        qInfo() << "\n[SQL][SUCCESS][CREATE][Settings]\n";

        //table is being created for the first time
        //add dummy values
        sqlQuery.finish();

        if(sqlQuery.exec("INSERT INTO Settings (access_code, code_status, checkbox_value, json_webtoken) "
                         "VALUES ('access_code_placeholder', 0, 0, 'json_web_token_placeholder')")) {

            qInfo() << "\n[SQL][SUCCESS][INSERT][Settings]\n";
        }
        else {
            qInfo() << "\n[SQL][FAILURE][INSERT][Settings]\n";
            qInfo() << sqlQuery.lastError();
        }
    }
    else {
        qInfo() << "\n[SQL][FAILURE][CREATE][Settings]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();

    if(sqlQuery.exec("CREATE TABLE Locations ( "
                        "index_entry INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                        "json_location TEXT NOT NULL "
                     ")")) {

        qInfo() << "\n[SQL][SUCCESS][CREATE][Locations]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][CREATE][Locations]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

//get the 4 stored user settings
QHash<QString, QString> DatabaseHelper::getSettings() {
    if(sqlQuery.exec("SELECT * FROM Settings")) {
        qInfo() << "\n[SQL][SUCCESS][SELECT][*][Settings]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][SELECT][*][Settings]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.first();

    QHash<QString, QString> hashmap;

    hashmap.insert("access_code", sqlQuery.value(0).toString());
    hashmap.insert("code_status", sqlQuery.value(1).toString());
    hashmap.insert("checkbox_value", sqlQuery.value(2).toString());
    hashmap.insert("json_webtoken", sqlQuery.value(3).toString());

    sqlQuery.finish();

    return hashmap;
}

//update / setter methods
void DatabaseHelper::updateAccessCode(const QString accessCode) {
    qInfo() << "[VALUE] access_code: " + accessCode;

    sqlQuery.prepare("UPDATE Settings SET access_code = ?");
    sqlQuery.addBindValue(accessCode);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][access_code]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][access_code]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateAccessCodeStatus(const bool codeStatus) {
    qInfo() << "[VALUE] code_status: " + QString::number(codeStatus);

    sqlQuery.prepare("UPDATE Settings SET code_status = ?");
    sqlQuery.addBindValue(codeStatus);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][code_status]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][code_status]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateCheckboxValue(const int checkBoxValue) {
    qInfo() << "[VALUE] checkbox_value: " + QString::number(checkBoxValue);

    sqlQuery.prepare("UPDATE Settings SET checkbox_value = ?");
    sqlQuery.addBindValue(checkBoxValue);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][checkbox_value]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][checkbox_value]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::updateJsonWebToken(const QString jsonWebToken) {
    qInfo() << "[VALUE] json_webtoken: "+ jsonWebToken;

    sqlQuery.prepare("UPDATE Settings SET json_webtoken = ?");
    sqlQuery.addBindValue(jsonWebToken);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SQL][SUCCESS][UPDATE][Settings][json_webtoken]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][UPDATE][Settings][json_webtoken]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

void DatabaseHelper::saveLocation(QString jsonString) {
    //store the coordinates in the database
    sqlQuery.prepare("INSERT INTO Locations (json_location) VALUES (?)");
    sqlQuery.addBindValue(jsonString);

    if(sqlQuery.exec()) {
        qInfo() << "\n[SQL][SUCCESS][INSERT][Locations]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][INSERT][Locations]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}

QVector<QString> DatabaseHelper::getSavedLocations() {
    if(sqlQuery.exec("SELECT json_location FROM Locations")) {
        qInfo() << "\n[SQL][SUCCESS][SELECT][json_location][Locations]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][SELECT][json_location][Locations]\n";
        qInfo() << sqlQuery.lastError();
    }

    QVector<QString> locations;

    sqlQuery.first();

    while(sqlQuery.next()) {
        locations.append(sqlQuery.value(0).toString());
    }

    sqlQuery.finish();

    return locations;
}

void DatabaseHelper::clearSavedLocations() {
    if(sqlQuery.exec("DELETE FROM Locations")) {
        qInfo() << "\n[SQL][SUCCESS][DELETE][*][Locations]\n";
    }
    else {
        qInfo() << "\n[SQL][FAILURE][DELETE][*][Locations]\n";
        qInfo() << sqlQuery.lastError();
    }

    sqlQuery.finish();
}
