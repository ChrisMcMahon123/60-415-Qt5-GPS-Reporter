#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>
#include <QtLocation/QLocation>
#include <QtPositioning/QGeoLocation>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QGeoPositionInfo>

class BackEnd : public QObject {
    Q_OBJECT
        Q_PROPERTY(QString connectionString READ connectionString WRITE setConnectionString NOTIFY connectionStringChanged)
        Q_PROPERTY(int connectionFlag READ connectionFlag)
        Q_PROPERTY(QString gpsLocation READ gpsLocation WRITE setGpsLocation)
        //TODO::
        //add framework to send location data from QML to C++
        //get internet working and server communication
public:
    explicit BackEnd(QObject *parent = nullptr);

    //getter methods
    QString connectionString();
    QString gpsLocation();
    int connectionStringStatus();
    bool sendGpsDataStatus();

    //setter methods
    void setConnectionString(const QString &serverString);
    void setGpsLocation(const QGeoPositionInfo position);

signals:
    void connectionStringChanged();
    void gpsLocationDataSent();

private:
    void updateServerString();
    void sendGpsData();
    void createDatabase(bool dropTablesFlag);

    QSqlDatabase db;
    QSqlQuery query;
    QString sqlQuery;
    QString serverString;
    QString timestamp;
    QString coordinates;
    int stringStatus;
    bool sendFlag;
};
#endif // BACKEND_H
