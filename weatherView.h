#ifndef WEATHERVIEW_H
#define WEATHERVIEW_H

#include <QObject>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <unordered_map>

class WeatherViewBackend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool connectionState READ getConnectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(int maxTemperature READ getMaxTemperature NOTIFY updateMaxTemperature);
    Q_PROPERTY(int minTemperature READ getMinTemperature NOTIFY updateMinTemperature);
    Q_PROPERTY(int currentTemperature READ getCurrentTemperature NOTIFY updateCurrentTemperature);
    Q_PROPERTY(bool isDaytime READ getDaytime NOTIFY updateDaytime);
    Q_PROPERTY(QString currentWeatherString READ getCurrentWeatherString NOTIFY updateCurrentWeatherString);
    Q_PROPERTY(CurrentWeatherTypes weatherType READ getWeatherType NOTIFY updateCurrentWeatherType);
    public:
        enum CurrentWeatherTypes {
            Unspecified,
            Clear,
            Cloudy,
            Windy,
            Showers,
            Rainy,
            Snowy,
            RainAndSnowMix,
            Hail,
            IsolatedThunderstorms,
        };
        Q_ENUM(CurrentWeatherTypes)

    public:
        explicit WeatherViewBackend(QObject *parent = nullptr, QNetworkAccessManager* nam = nullptr);

        void requestData();

        void receiveReply(QNetworkReply* rply);

        enum ConnectionStates {
            NO_CONNECTION,
            CONNECTED_INFO,
        };

        Q_INVOKABLE bool getConnectionState() {
            if (this->states == CONNECTED_INFO) {
                return true;
            } else {
                return false;
            }
        };

        void setMaxTemperature(int maxTemp);
        void setMinTemperature(int minTemp);
        void setCurrentTemperature(int curTemp);
        void setCurrentWeatherString(QString curWeather, QString weatherType);
        void setDaytime(bool daytime);

        int getMaxTemperature() {return this->maxTemperature;};
        int getMinTemperature() {return this->minTemperature;};
        int getCurrentTemperature() {return this->currentTemperature;};
        QString getCurrentWeatherString() {return this->currentWeatherString;};
        CurrentWeatherTypes getWeatherType() const {return this->currentWeatherCategory;};
        Q_INVOKABLE bool getDaytime() {
            qDebug() << "daytime is: " << this->isDaytime;
            return this->isDaytime;}

    private:

        QNetworkAccessManager* m_nam;
        QTimer m_weatherTimer;
        QString m_apiUrl;

        ConnectionStates states;
        QString currentWeatherString;
        CurrentWeatherTypes currentWeatherCategory;
        int currentTemperature;
        int maxTemperature;
        int minTemperature;
        bool isDaytime;




        std::unordered_map<QString, CurrentWeatherTypes> weatherCategory = {
            {"TYPE_UNSPECIFIED", Unspecified},
            {"CLEAR", Clear},
            {"MOSTLY_CLEAR", Clear},
            {"PARTLY_CLOUDY", Cloudy},
            {"MOSTLY_CLOUDY", Cloudy},
            {"CLOUDY", Cloudy},
            {"WINDY", Windy},
            {"WIND_AND_RAIN", Windy},
            {"LIGHT_RAIN_SHOWERS", Showers},
            {"CHANCE_OF_SHOWERS", Showers},
            {"SCATTERED_SHOWERS", Showers},
            {"RAIN_SHOWERS", Showers},
            {"HEAVY_RAIN_SHOWERS", Showers},
            {"LIGHT_TO_MODERATE_RAIN", Rainy},
            {"MODERATE_TO_HEAVY_RAIN", Rainy},
            {"RAIN", Rainy},
            {"LIGHT_RAIN", Rainy},
            {"HEAVY_RAIN", Rainy},
            {"RAIN_PERIODICALLY_HEAVY", Rainy},
            {"LIGHT_SNOW_SHOWERS", Snowy},
            {"CHANCE_OF_SNOW_SHOWERS", Snowy},
            {"SCATTERED_SNOW_SHOWERS", Snowy},
            {"SNOW_SHOWERS", Snowy},
            {"HEAVY_SNOW_SHOWERS", Snowy},
            {"LIGHT_TO_MODERATE_SNOW", Snowy},
            {"MODERATE_TO_HEAVY_SNOW", Snowy},
            {"SNOW", Snowy},
            {"LIGHT_SNOW", Snowy},
            {"HEAVY_SNOW", Snowy},
            {"SNOWSTORM", Snowy},
            {"SNOW_PERIODICALLY_HEAVY", Snowy},
            {"HEAVY_SNOW_STORM", Snowy},
            {"BLOWING_SNOW", Snowy},
            {"RAIN_AND_SNOW", RainAndSnowMix},
            {"HAIL", Hail},
            {"HAIL_SHOWERS", Hail},
            {"THUNDERSTORM", IsolatedThunderstorms},
            {"THUNDERSHOWER", IsolatedThunderstorms},
            {"LIGHT_THUNDERSTORM_RAIN", IsolatedThunderstorms},
            {"SCATTERED_THUNDERSTORMS", IsolatedThunderstorms},
            {"HEAVY_THUNDERSTORM", IsolatedThunderstorms},
        };

    signals:
        void connectionStateChanged();
        void updateMaxTemperature();
        void updateMinTemperature();
        void updateCurrentTemperature();
        void updateDaytime();
        void updateCurrentWeatherString();
        void updateCurrentWeatherType();

};


#endif // WEATHERVIEW_H
