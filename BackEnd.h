#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>
#include <QHash>

#include "Databasehelper.h"

class BackEnd : public QObject {
    Q_OBJECT
        Q_PROPERTY(QString connectionString READ connectionString WRITE setConnectionString NOTIFY connectionStringChanged)
        Q_PROPERTY(int connectionStringStatus READ connectionStringStatus)
        Q_PROPERTY(QString gpsLocation READ gpsLocation WRITE setGpsLocation NOTIFY gpsLocationDataSent)
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
    void setConnectionString(const QString &newString);
    void setGpsLocation(const QString &gpsInformation);

signals:
    void connectionStringChanged();
    void gpsLocationDataSent();

private:
    void updateServerString();
    void sendGpsData();
    void createDatabase(bool dropTablesFlag);

    DatabaseHelper databaseHelper;
    QString serverString;
    QString timestamp;
    QString coordinates;
    QString longitude;
    QString latitude;
    int stringStatus;
    bool sendFlag;
};
#endif // BACKEND_H
