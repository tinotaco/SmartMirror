#include "calendarEvents.h"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QOAuthHttpServerReplyHandler>

#include <QThread>


constexpr const char* authorizationUrl = "https://accounts.google.com/o/oauth2/v2/auth";
constexpr const char* accessTokenUrl = "https://oauth2.googleapis.com/token";
constexpr const char* scopeCalendarList = "https://www.googleapis.com/auth/calendar.readonly";

constexpr const char* calendarListApi = "https://www.googleapis.com/calendar/v3/users/me/calendarList";
constexpr const char* calendarEventApi = "https://www.googleapis.com/calendar/v3/calendars/%1/events";

constexpr uint REFRESHRATE = 3600000; //Refreshes calendar events every hour


CalendarEventsBackend::CalendarEventsBackend(QObject *parent, QNetworkAccessManager* nam): m_nam(nam), connectionState(NOTCONNECTED) {
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
        configPath = QDir::currentPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
        configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.ini";

    QSettings settings(configPath, QSettings::IniFormat);
    const QString clientIdentifier = settings.value("calendar/clientId").toString().trimmed();
    const QString clientSecret = settings.value("calendar/clientSecret").toString().trimmed();
    m_calendarSummary = settings.value("calendar/summary").toString().trimmed();
    if (clientIdentifier.isEmpty() || clientSecret.isEmpty()) {
        qWarning() << "Calendar OAuth credentials are missing from" << configPath;
        return;
    }

    m_oauth = new QOAuth2AuthorizationCodeFlow;
    m_oauth->setRequestedScopeTokens({scopeCalendarList});
    connect(m_oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            &QDesktopServices::openUrl);
    m_oauth->setAuthorizationUrl(QUrl(authorizationUrl));
    m_oauth->setClientIdentifier(clientIdentifier);
    m_oauth->setTokenUrl(QUrl(accessTokenUrl));
    m_oauth->setClientIdentifierSharedKey(clientSecret);

    auto replyHandler = new QOAuthHttpServerReplyHandler(1234, this);
    replyHandler->setCallbackPath("/cb");
    m_oauth->setReplyHandler(replyHandler);
    m_oauth->grant();

    //connect(this->m_oauth, &QOAuth2AuthorizationCodeFlow::granted, this, &CalendarEventsBackend::sendServerEventsUpdateRequest);
    connect(this->m_oauth, &QOAuth2AuthorizationCodeFlow::granted, this, &CalendarEventsBackend::getCalendarFromApi);
    connect(&m_serverUpdateTimer, &QTimer::timeout, this, &CalendarEventsBackend::sendServerEventsUpdateRequest);
    this->m_serverUpdateTimer.start(REFRESHRATE);
}



void CalendarEventsBackend::sendServerEventsUpdateRequest() {
    //Do not send request if a connection has not been established (calendarId will be missing)
    if (this->connectionState != CONNECTED) {
        qDebug() << "Connection State of Calendar Events does not equal CONNECTED";
        return;
    }
    //Prepare Network Request with previously found Calendar Id
    QUrl calendarEventsUrl = QUrl(QString(calendarEventApi).arg(QUrl::toPercentEncoding(this->calendarId)));

    //Add query
    QUrlQuery query;
    query.addQueryItem("maxResults", QString::number(MAXEVENTRESULTS));
    query.addQueryItem("singleEvents", "true");
    query.addQueryItem("orderBy", "startTime"); //orderBy can only be used in combination with singleEvents=true
    query.addQueryItem("timeMin", QDateTime::currentDateTime().toString("yyyy-MM-ddT00:00:00.000Z"));
    calendarEventsUrl.setQuery(query);
    //Add query arguments and prepare request
    QNetworkRequest req = QNetworkRequest(calendarEventsUrl);
    this->m_oauth->prepareRequest(&req, "GET");

    //Network Reply
    QNetworkReply* reply = this->m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {receiveEventsReply(reply);});
    qDebug() << "Sent the calendar events request";
}

void CalendarEventsBackend::getCalendarFromApi() {
    QNetworkRequest req = QNetworkRequest(QUrl(calendarListApi));
    this->m_oauth->prepareRequest(&req, "GET");

    QNetworkReply* reply = this->m_nam->get(req);
    qDebug() << "Reply state immediately after get:"
             << reply->isRunning()
             << reply->isFinished();
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            return;
        }
        //qDebug() << "Have received calendar event: " << reply->readAll();
        QByteArray tempData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(tempData);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray calendarArray = jsonObj["items"].toArray();
        for (const QJsonValue& value : calendarArray) {
            QJsonObject item = value.toObject();
            QString calendarName = item["summary"].toString();
            if (calendarName == m_calendarSummary) {
                this->calendarId = item["id"].toString();
                reply->deleteLater();
                this->connectionState = CONNECTED;
                this->sendServerEventsUpdateRequest();
                return;
            }
        }
        qDebug() << "Calendar has not been found";
        //Set state to CONNECTED if everything passes (TODO: add failure conditions)
        reply->deleteLater();
    });
}


void CalendarEventsBackend::receiveEventsReply(QNetworkReply* rply) {
    if (rply->error() != QNetworkReply::NoError) {
        qDebug() << "Error receiving Calendar Events Reply:";
        this->connectionState = NOTCONNECTED;
        return;
    }
    qDebug() << "Received Events Reply";
    QByteArray tempData = rply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(tempData);
    QJsonObject jsonObj = jsonDoc.object();

    //TODO: need to detect more failure conditions

    //Reset the list
    this->eventsList.clear();

    //Set Calendar Events
    QJsonArray calendarEventsArray = jsonObj["items"].toArray();
    for (int i = 0; i < calendarEventsArray.size() && eventsList.size() < MAXEVENTRESULTS; i++) {
        QJsonValue value = calendarEventsArray.at(i);
        QJsonObject item = value.toObject();
        if (item.isEmpty())
            continue;

        //Prepare New List Entry Values
        const QJsonObject startObject = item.value("start").toObject();
        const QJsonObject endObject = item.value("end").toObject();
        const QString startDateTimeString = startObject.value("dateTime").toString();
        const QString endDateTimeString = endObject.value("dateTime").toString();
        const bool isTimedEvent = !startDateTimeString.isEmpty() && !endDateTimeString.isEmpty();
        QDateTime startTime = QDateTime::fromString(startDateTimeString, Qt::ISODate);
        QDateTime endTime = QDateTime::fromString(endDateTimeString, Qt::ISODate);
        QString description = item.value("summary").toString();
        QDate startDay = isTimedEvent ? startTime.date() : QDate::fromString(startObject.value("date").toString(), Qt::ISODate);
        QDate endDay = isTimedEvent ? endTime.date() : QDate::fromString(endObject.value("date").toString(), Qt::ISODate);
        EventType eventType;
        if (description.isEmpty()) {
            eventType = NOEVENT;
        } else if (!isTimedEvent) {
            eventType = startDay == QDate::currentDate() ? ALLDAY : UPCOMING;
        } else if (!startTime.isValid() || !endTime.isValid() || !startDay.isValid()) {
            qWarning() << "Invalid timed calendar event:" << item;
            eventType = NOEVENT;
        } else if (startDay == QDate::currentDate()) {
            eventType = APPOINTMENT;
        } else {
            eventType = UPCOMING;
        }

        //Add List Entry
        eventsList.emplace_back(eventType, startTime, startDay, endTime, endDay, description);
    }
    emit updateCalendarEvents();
}