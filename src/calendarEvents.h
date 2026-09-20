#ifndef CALENDAREVENTS_H
#define CALENDAREVENTS_H

#include <QObject>
#include <QString>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QOAuthUriSchemeReplyHandler>
#include <QOAuth2AuthorizationCodeFlow>
#include <QNetworkRequestFactory>
#include <QNetworkReply>

#include <vector>

constexpr uint MAXEVENTRESULTS = 3; //Amount of calendar events to be returned

class CalendarEventsBackend : public QObject {
    Q_OBJECT

    enum ConnectionStatus {
        NOTCONNECTED,
        CONNECTED,
    };

    enum EventType {
        NOEVENT,
        APPOINTMENT, //Event on current day with start and stop time
        ALLDAY, //Event on current day without start and stop time
        UPCOMING, //Events in the future
    };
    Q_ENUM(EventType);

    struct CalendarEntry {
        EventType eventType;
        QDateTime startTime;
        QDate startDay;
        QDateTime endTime;
        QDate endDay;
        QString descriptionString;
        int daysUntil;

        CalendarEntry(EventType event, QDateTime strtTime, QDate startDay,
                    QDateTime endTime, QDate endDay, QString dscrptionString) :
            eventType(event), startTime(strtTime), startDay(startDay),
            endTime(endTime), endDay(endDay), descriptionString(dscrptionString) {

            daysUntil = QDate::currentDate().daysTo(startDay);

            qDebug() << "Added Event with: starttime " << startTime.toString() <<
                "startDay: " << startDay.toString() << "endtime: " << endTime.toString() <<
                "endDay: " << endDay.toString() << "description: " << dscrptionString <<
                "eventType: " << event << "days until: " << daysUntil;
        };
    };

    public:
        explicit CalendarEventsBackend(QObject *parent = nullptr, QNetworkAccessManager* nam = nullptr);

        void sendRequest();
        void receiveEventsReply(QNetworkReply* rply);
        void getCalendarFromApi();
        void sendServerEventsUpdateRequest();

        Q_INVOKABLE QVariantMap returnCalendarEventEntry(int index) {
            QVariantMap map;
            if (index < 0 || index >= MAXEVENTRESULTS) {
                map["eventType"] = NOEVENT;
                map["descriptionString"] = "Error";
                return map;
            }
            map["eventType"] = this->eventsList.at(index).eventType;
            map["descriptionString"] = this->eventsList.at(index).descriptionString;
            map["startDay"] = this->eventsList.at(index).startDay;
            map["startTime"] = this->eventsList.at(index).startTime;
            map["endTime"] = this->eventsList.at(index).endTime;
            map["daysUntil"] = this->eventsList.at(index).daysUntil;
            return map;
        }



    private:
        ConnectionStatus connectionState;
        QNetworkAccessManager* m_nam;
        QOAuth2AuthorizationCodeFlow* m_oauth = nullptr;
        QOAuthUriSchemeReplyHandler* m_handler;
        QNetworkRequestFactory* m_api;


        QString calendarId;
        QString m_calendarSummary;
        QTimer m_serverUpdateTimer;

        std::vector<CalendarEntry> eventsList;

    signals:
        void updateCalendarEvents();


};



#endif // CALENDAREVENTS_H
