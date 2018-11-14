#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHash>
#include <QJsonObject>

class DatabaseHelper {
public:
    //constructors
    DatabaseHelper();
    DatabaseHelper(bool flag);

    //setters
    void updateAccessCode(QString accessCode);
    void updateAccessCodeStatus(bool codeStatus);
    void updateCheckboxValue(int checkBoxValue);
    void updateJsonWebToken(QString jsonWebToken);
    void saveLocation(QString jsonString);

    //getters
    QHash<QString, QString> getSettings();
    QVector<QString> getSavedLocations();

    //destroy
    void clearSavedLocations();

private:
    void createDatabase(bool dropTablesFlag);

    QSqlDatabase database;
    QSqlQuery sqlQuery;
};

#endif // DATABASEHELPER_H
