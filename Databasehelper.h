#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHash>

class DatabaseHelper {
    public:
        DatabaseHelper();

        void insertLocation(const QString &latitude, const QString &longitude, const QString &timestamp);
        void insertServerKey(QString serverString, int stringStatus);
        QHash<QString, QString> getServerKey();

    private:
        void createDatabase(bool dropTablesFlag);

        QSqlDatabase database;
        QSqlQuery sqlQuery;
        QString queryString;
};

#endif // DATABASEHELPER_H
