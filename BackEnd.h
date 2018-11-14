#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>
#include <QHash>
#include <QJsonObject>
#include <QtNetwork>

#include "Databasehelper.h"

class BackEnd : public QObject {
    Q_OBJECT
        Q_PROPERTY(QString serverAccessCode READ serverAccessCode WRITE setServerAccessCode NOTIFY serverAccessCodeChanged)
        Q_PROPERTY(QString locationData READ locationData WRITE setLocationData NOTIFY locationDataSent)

        Q_PROPERTY(bool serverAccessCodeStatus READ serverAccessCodeStatus NOTIFY serverAccessCodeStatusChanged)
        Q_PROPERTY(bool locationDataSentStatus READ locationDataSentStatus)
        Q_PROPERTY(int checkboxStatus READ checkboxStatus WRITE setCheckboxStatus NOTIFY checkboxStatusChanged)
        Q_PROPERTY(bool networkConnectionStatus READ networkConnectionStatus NOTIFY networkConnectionStatusChanged)

    //TODO::
    //get internet working and server communication
public:
    explicit BackEnd(QObject *parent = nullptr);

    //getter methods
    QString serverAccessCode();
    QString locationData();

    bool serverAccessCodeStatus();
    bool locationDataSentStatus();
    bool networkConnectionStatus();
    int checkboxStatus();

    //setter methods
    void setServerAccessCode(const QString &newCode);
    void setLocationData(const QString &locationInformation);
    void setCheckboxStatus(const int &checkBoxValue);

signals:
    void serverAccessCodeChanged();
    void locationDataSent();
    void checkboxStatusChanged();//prevent errors
    void networkConnectionStatusChanged();
    void serverAccessCodeStatusChanged();//prevent errors
private:
    DatabaseHelper databaseHelper;

    QJsonObject json;
    QJsonObject jsonInner;
    QJsonDocument jsonDocument;
    QString jsonString;

    QByteArray jsonData;
    QNetworkRequest request;
    QNetworkAccessManager *networkManager;

    QString accessCode;
    QString jsonWebToken;
    QString timestamp;
    QString coordinates;
    QString longitude;
    QString latitude;

    bool accessCodeStatusFlag;
    bool sendLocationDataFlag;
    int checkboxValue;
    bool connectionStatusFlag;
};
#endif // BACKEND_H
