#include "BackEnd.h"

BackEnd::BackEnd(QObject *parent) : QObject(parent) {
    //sending 'true' to the constructor will drop all tables
    //and recreate them, all data will be lost
    databaseHelper = DatabaseHelper(true);

    //get accessCode, accessCodeStatus and checkbox value from the database
    QHash<QString, QString> hashmap = databaseHelper.getSettings();

    accessCode = hashmap["access_code"];
    accessCodeStatusFlag = hashmap["code_status"].toInt();
    checkboxValue = hashmap["checkbox_value"].toInt();
    jsonWebToken = hashmap["json_webtoken"];

    qInfo() << "[access_code]: " + accessCode + "\n";
    qInfo() << "[code_status]: " + QString::number(accessCodeStatusFlag)+ "\n";
    qInfo() << "[checkbox_value]: " + QString::number(checkboxValue) + "\n";
    qInfo() << "[json_webtoken]: " + jsonWebToken + "\n";

    //network check - connect to google.ca
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("www.google.ca", 80);

    if(socket->waitForConnected(5000)) {
        connectionStatusFlag = true;
    }
    else {
        connectionStatusFlag = false;
    }
}

//getter methods
QString BackEnd::serverAccessCode() {
    return accessCode;
}

QString BackEnd::locationData() {
    return "";
}

bool BackEnd::serverAccessCodeStatus() {
    return accessCodeStatusFlag;
}

bool BackEnd::locationDataSentStatus() {
    return sendLocationDataFlag;
}

bool BackEnd::networkConnectionStatus() {
    return connectionStatusFlag;
}

int BackEnd::checkboxStatus() {
    return checkboxValue;
}

//setter methods
void BackEnd::setServerAccessCode(const QString &newCode) {
    if(newCode != "") {
        accessCode = newCode;

        //perform a server test with the new string
        //TODO::
        accessCodeStatusFlag = true;

        databaseHelper.updateAccessCode(accessCode);
        databaseHelper.updateAccessCodeStatus(accessCodeStatusFlag);
        databaseHelper.updateJsonWebToken("JSON WEBTOKEN NEW UPDATED VALUE");

        emit serverAccessCodeChanged();
    }
}

void BackEnd::setCheckboxStatus(const int &newValue) {
    checkboxValue = newValue;
    databaseHelper.updateCheckboxValue(checkboxValue);
}

void BackEnd::setLocationData(const QString &locationInformation) {
    if(locationInformation.length() > 1) {
        //parse the string to get the latitude, longitude and timestamp
        coordinates = locationInformation.left(locationInformation.indexOf("|"));
        timestamp = locationInformation.mid(locationInformation.indexOf("|") + 1);

        latitude = coordinates.left(coordinates.indexOf(" "));
        longitude = coordinates.mid(coordinates.indexOf(" ") + 1);

        //convert the datetime format from UTC to MYSQL format
        QDateTime datetime = QDateTime::fromString(timestamp);
        timestamp = datetime.toString("yyyy-MM-dd hh:mm:ss");

        jsonInner = QJsonObject();
        jsonInner["access_code"] = accessCode;
        jsonInner["latitude"] = latitude;
        jsonInner["longitude"] = longitude;
        jsonInner["timestamp"] = timestamp;

        json = QJsonObject();
        json["device"] = jsonInner;

        //convert JSON to store in database or send over network
        jsonDocument = QJsonDocument(json);
        jsonString = jsonDocument.toJson(QJsonDocument::Compact);

        qInfo() << "JSON STRING COMPRESSED: " << jsonString;

        //network check
        QTcpSocket *socket = new QTcpSocket();
        socket->connectToHost("www.google.ca", 80);

        if(socket->waitForConnected(5000)) {
            connectionStatusFlag = true;
            emit networkConnectionStatusChanged();
        }
        else {
            connectionStatusFlag = false;
            emit networkConnectionStatusChanged();
        }

        if(accessCodeStatusFlag && connectionStatusFlag) {
            //access code is valid and there is an internet connection
            //send current location and any stored locations, delete the stored locations after
            QUrl serviceUrl = QUrl("http://192.168.1.136:3002/location");

            //send current location
            jsonData = jsonDocument.toJson(QJsonDocument::Compact);

            //set headers
            request = QNetworkRequest(serviceUrl);
            request.setRawHeader("Authorization", "Bearer: TOKEN_HERE");
            request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
            request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

            networkManager = new QNetworkAccessManager(this);
            QObject::connect(networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onLocationReply(QNetworkReply *)));

            // Send as POST (regardless if we know connection is established)
            networkManager->post(request, jsonData);

            //send stored locations
            QVector<QString> locations = databaseHelper.getSavedLocations();

            for(int i = 0; i < locations.size(); i++) {
                qInfo() << "Location: " + QString::number(i) + " = " + locations.at(i);
                jsonData = locations.at(i).toUtf8();

                //set headers
                request = QNetworkRequest(serviceUrl);
                request.setRawHeader("Authorization", "Bearer: TOKEN_HERE");
                request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
                request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

                networkManager = new QNetworkAccessManager(this);
                QObject::connect(networkManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(onLocationReply(QNetworkReply *)));

                // Send as POST (regardless if we know connection is established)
                networkManager->post(request, jsonData);
            }

            sendLocationDataFlag = true;

            //all data sent successfully, delete the stored locations
            databaseHelper.clearSavedLocations();

            emit locationDataSent();
        }
        else if(accessCodeStatusFlag) {
            //access code is valid but there is no internet connection
            //store the location json file in the database
            databaseHelper.saveLocation(jsonString);

            sendLocationDataFlag = false;

            emit locationDataSent();
        }
        else {
            //no access code set, dont store any location data
            qInfo() << "No location data stored or sent";
        }
    }
}
