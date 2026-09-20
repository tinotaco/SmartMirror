#include "weatherView.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QStandardPaths>
#include <QUrlQuery>

constexpr uint REFRESHRATE = 3600000; //Refreshes weather data every hour

WeatherViewBackend::WeatherViewBackend(QObject *parent, QNetworkAccessManager* nam) : m_nam(nam), states(NO_CONNECTION) {
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
        configPath = QDir::currentPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
        configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.ini";

    QSettings settings(configPath, QSettings::IniFormat);
    const QString apiKey = settings.value("weather/apiKey").toString().trimmed();
    const double latitude = settings.value("weather/latitude", 48.0265).toDouble();
    const double longitude = settings.value("weather/longitude", 11.5898).toDouble();
    QUrlQuery query;
    query.addQueryItem("key", apiKey);
    query.addQueryItem("location.latitude", QString::number(latitude, 'f', 6));
    query.addQueryItem("location.longitude", QString::number(longitude, 'f', 6));
    QUrl weatherUrl("https://weather.googleapis.com/v1/currentConditions:lookup");
    weatherUrl.setQuery(query);
    m_apiUrl = weatherUrl.toString();
    if (apiKey.isEmpty())
        qWarning() << "Weather API key is missing from" << configPath;

    connect(&m_weatherTimer, &QTimer::timeout, this, &WeatherViewBackend::requestData);
    m_weatherTimer.start(REFRESHRATE);

    this->requestData();
}

void WeatherViewBackend::setDaytime(bool daytime) {
    if (daytime != this->isDaytime) {
        this->isDaytime = daytime;
        qDebug() << "Daytime set to " << daytime;
        emit updateDaytime();
    }
}

void WeatherViewBackend::setMaxTemperature(int maxTemp) {
    if (maxTemp != this->maxTemperature) {
        this->maxTemperature = maxTemp;
        emit updateMaxTemperature();
    }
}

void WeatherViewBackend::setMinTemperature(int minTemp) {
    if (minTemp != this->minTemperature) {
        this->minTemperature = minTemp;
        emit updateMinTemperature();
    }
}

void WeatherViewBackend::setCurrentTemperature(int curTemp) {
    if (curTemp != this->currentTemperature) {
        this->currentTemperature = curTemp;
        emit updateCurrentTemperature();
    }
}

void WeatherViewBackend::setCurrentWeatherString(QString curWeather, QString weatherType) {
    if (curWeather != this->currentWeatherString) {
        this->currentWeatherString = curWeather;
        this->currentWeatherCategory = weatherCategory[weatherType];

        emit updateCurrentWeatherString();
        emit updateCurrentWeatherType();
    }
}


void WeatherViewBackend::requestData() {
    QNetworkReply* reply = this->m_nam->get(QNetworkRequest(QUrl(m_apiUrl)));

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {receiveReply(reply);});

    qDebug() << "Sent Request Data";
}

void WeatherViewBackend::receiveReply(QNetworkReply *rply) {
    qDebug() << "Received Reply";
    if (rply->error() != QNetworkReply::NoError) {
        qDebug() << "Error receiving Weather Reply:";
        this->states = NO_CONNECTION;
        emit connectionStateChanged();
        return;
    }
    QByteArray tempData = rply->readAll();
    // parse data here, NOT reply

    QJsonDocument jsonDoc = QJsonDocument::fromJson(tempData);
    QJsonObject jsonObj = jsonDoc.object();

    //Update Current temperature
    int currTemp = jsonObj.value("temperature").toObject().value("degrees").toDouble();
    this->setCurrentTemperature(currTemp);

    //Update Min Temperature over last 24 hours
    int minTemp = jsonObj.value("currentConditionsHistory").toObject().value("minTemperature").toObject().value("degrees").toDouble();
    this->setMinTemperature(minTemp);

    //Update Max Temperature over last 24 hours
    int maxTemp = jsonObj.value("currentConditionsHistory").toObject().value("maxTemperature").toObject().value("degrees").toDouble();
    this->setMaxTemperature(maxTemp);

    //Update Daytime Boolean
    bool daytime = jsonObj.value("isDaytime").toBool();
    this->setDaytime(daytime);

    //Update Weather Text String and Categorize
    QString weatherString = jsonObj.value("weatherCondition").toObject().value("description").toObject().value("text").toString();
    QString weatherType = jsonObj.value("weatherCondition").toObject().value("type").toString();
    this->setCurrentWeatherString(weatherString, weatherType);

    //Change State of Connection in UI and update variable
    if (this->states == NO_CONNECTION) {
        this->states = CONNECTED_INFO;
        connectionStateChanged();
    }
}
