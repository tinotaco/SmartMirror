#include "departureTableView.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QTime>
#include <QUrl>
#include <QXmlStreamReader>
#include <algorithm>

constexpr const char *PLAN_URL = "https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/plan/%1/%2/%3";
constexpr const char *PLANNED_CHANGES_URL = "https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/fchg/%1";
constexpr const char *RECENT_CHANGES_URL = "https://apis.deutschebahn.com/db-api-marketplace/apis/timetables/v1/rchg/%1";
constexpr int REFRESH_INTERVAL_MS = 60000;

DepartureTableView::DepartureTableView(QObject *parent, QNetworkAccessManager *nam)
    : QObject(parent), m_nam(nam) {
    connect(&m_refreshTimer, &QTimer::timeout, this, &DepartureTableView::requestDepartures);
    if (loadConfig()) {
        m_refreshTimer.start(REFRESH_INTERVAL_MS);
        requestDepartures();
    }
}

QVariantList DepartureTableView::departures() const {
    return m_departures;
}

QString DepartureTableView::status() const {
    return m_status;
}

void DepartureTableView::setStatus(const QString &status) {
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

bool DepartureTableView::loadConfig() {
    QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
        configPath = QDir::currentPath() + "/config.ini";
    if (!QFileInfo::exists(configPath))
        configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.ini";

    QSettings settings(configPath, QSettings::IniFormat);
    m_evaNumber = settings.value("station/evaNumber").toString().trimmed();
    m_clientId = settings.value("db/clientId").toString().trimmed();
    m_apiKey = settings.value("db/apiKey").toString().trimmed();
    qInfo().noquote() << "[DepartureTable] Config path:" << configPath
                      << "EVA:" << m_evaNumber
                      << "Client ID configured:" << !m_clientId.isEmpty()
                      << "API key configured:" << !m_apiKey.isEmpty();
    if (m_evaNumber.isEmpty() || m_clientId.isEmpty() || m_apiKey.isEmpty()) {
        qWarning() << "[DepartureTable] Missing station or DB API configuration";
        setStatus("Configure station/evaNumber and DB API credentials");
        return false;
    }
    return true;
}

void DepartureTableView::requestDepartures() {
    if (m_refreshInProgress)
        return;

    m_refreshInProgress = true;
    m_pendingRequests = 4;
    m_timetableList.clear();
    setStatus("Updating departures...");

    QDateTime currentHour = QDateTime::currentDateTime();
    currentHour.setTime(QTime(currentHour.time().hour(), 0));
    qInfo().noquote() << "[DepartureTable] Requesting departures for local hours"
                      << currentHour.toString(Qt::ISODate)
                      << "and" << currentHour.addSecs(3600).toString(Qt::ISODate)
                      << "Current time:" << QDateTime::currentDateTime().toString(Qt::ISODate);
    requestPlan(currentHour);
    requestPlan(currentHour.addSecs(3600));
    requestChanges(QString(PLANNED_CHANGES_URL).arg(m_evaNumber), "planned changes");
    requestChanges(QString(RECENT_CHANGES_URL).arg(m_evaNumber), "recent changes");
}

void DepartureTableView::requestPlan(const QDateTime &hour) {
    const QString url = QString(PLAN_URL).arg(m_evaNumber, hour.toString("yyMMdd"), hour.toString("HH"));
    qInfo().noquote() << "\n[DepartureTable] GET" << url << "\n";
    QNetworkRequest request{QUrl(url)};
    request.setRawHeader("DB-Api-Key", m_apiKey.toUtf8());
    request.setRawHeader("DB-Client-Id", m_clientId.toUtf8());
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray response = reply->readAll();
        const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qInfo() << "[DepartureTable] Reply HTTP status:" << statusCode
                << "Network error:" << reply->error()
                << "Bytes:" << response.size();
        if (reply->error() != QNetworkReply::NoError)
            qWarning().noquote() << "[DepartureTable] Network error:" << reply->errorString();
        //qInfo().noquote() << "[DepartureTable] Response preview:\n"
        //                  << QString::fromUtf8(response.left(4000));

        const bool success = reply->error() == QNetworkReply::NoError && parsePlanXML(response);
        if (!success)
            setStatus("Unable to load departures");
        reply->deleteLater();
        --m_pendingRequests;
        if (m_pendingRequests == 0) {
            m_refreshInProgress = false;
            publishDepartures();
        }
    });
}

bool DepartureTableView::parsePlanXML(const QByteArray &data) {
    QXmlStreamReader xml(data);
    QString serviceId;
    QString arrivalTime;
    QString trainNumber;
    int parsedDepartures = 0;
    while (!xml.atEnd()) {
        xml.readNext();
        if (!xml.isStartElement())
            continue;

        if (xml.name() == QLatin1String("tl")) {
            trainNumber = xml.attributes().value("n").toString();
        } else if (xml.name() == QLatin1String("s")) {
            serviceId = xml.attributes().value("id").toString();
            arrivalTime.clear();
        } else if (xml.name() == QLatin1String("dp")) {
            const auto attributes = xml.attributes();
            const QString dbTimestamp = attributes.value("pt").toString();
            const QDateTime departure = QDateTime::fromString("20" + dbTimestamp, "yyyyMMddHHmm");
            if (!departure.isValid()) {
                qWarning().noquote() << "[DepartureTable] Invalid departure time:"
                                     << dbTimestamp;
                continue;
            }
            const QStringList path = attributes.value("ppth").toString().split('|', Qt::SkipEmptyParts);
            TimetableEntry entry;
            entry.serviceId = serviceId;
            entry.departure = departure;
            entry.liveDeparture = departure;
            entry.line = attributes.value("l").toString();
            entry.destination = path.isEmpty() ? QString() : path.last();
            entry.trainNumber = trainNumber;
            m_timetableList.append(entry);
            ++parsedDepartures;
            qInfo().noquote() << "[DepartureTable] Plan train id:" << serviceId
                              << "start:" << arrivalTime
                              << "end:" << departure.toString(Qt::ISODate)
                              << "line:" << entry.line
                              << "destination:" << entry.destination
                              << "train:" << entry.trainNumber;
        } else if (xml.name() == QLatin1String("ar")) {
            arrivalTime = xml.attributes().value("pt").toString();
        }
    }
    if (xml.hasError())
        qWarning().noquote() << "[DepartureTable] XML error:" << xml.errorString();
    qInfo() << "[DepartureTable] Parsed departures in response:" << parsedDepartures;
    return !xml.hasError();
}

void DepartureTableView::requestChanges(const QString &url, const QString &source) {
    qInfo().noquote() << "\n[DepartureTable] GET " << source << ":" << url << "\n";
    QNetworkRequest request{QUrl(url)};
    request.setRawHeader("DB-Api-Key", m_apiKey.toUtf8());
    request.setRawHeader("DB-Client-Id", m_clientId.toUtf8());
    QNetworkReply *reply = m_nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, source]() {
        const QByteArray response = reply->readAll();
        const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qInfo() << "[DepartureTable] " << source << " reply HTTP status:" << statusCode
                << "Network error:" << reply->error()
                << "Bytes:" << response.size();
        if (reply->error() != QNetworkReply::NoError)
            qWarning().noquote() << "[DepartureTable] " << source << " network error:" << reply->errorString();
        qInfo().noquote() << "[DepartureTable] " << source << " response preview:\n"
                          << QString::fromUtf8(response.left(4000));

        if (reply->error() == QNetworkReply::NoError && !parseChangesXML(response, source))
            setStatus("Unable to parse departure changes");
        reply->deleteLater();
        --m_pendingRequests;
        if (m_pendingRequests == 0) {
            m_refreshInProgress = false;
            publishDepartures();
        }
    });
}

bool DepartureTableView::parseChangesXML(const QByteArray &data, const QString &source) {
    QXmlStreamReader xml(data);
    QString serviceId;
    int matchedChanges = 0;
    while (!xml.atEnd()) {
        xml.readNext();
        if (!xml.isStartElement())
            continue;

        if (xml.name() == QLatin1String("s")) {
            serviceId = xml.attributes().value("id").toString();
            qInfo().noquote() << "[DepartureTable] " << source << " train id:" << serviceId;
        } else if (xml.name() == QLatin1String("m")) {
            const auto attributes = xml.attributes();
            qInfo().noquote() << "[DepartureTable] " << source << " train id:" << serviceId
                              << "start:" << attributes.value("from").toString()
                              << "end:" << attributes.value("to").toString()
                              << "category:" << attributes.value("cat").toString();
        } else if (xml.name() == QLatin1String("dp")) {
            const auto attributes = xml.attributes();
            const QString changedTime = attributes.value("ct").toString();
            const QDateTime liveDeparture = QDateTime::fromString("20" + changedTime, "yyyyMMddHHmm");
            for (TimetableEntry &entry : m_timetableList) {
                if (entry.serviceId != serviceId)
                    continue;
                if (liveDeparture.isValid())
                    entry.liveDeparture = liveDeparture;
                entry.cancelled = attributes.value("cs") == QLatin1String("c");
                ++matchedChanges;
                qInfo().noquote() << "[DepartureTable] " << source << " train id:" << serviceId
                                  << "start:" << attributes.value("ct").toString()
                                  << "end:" << entry.liveDeparture.toString(Qt::ISODate)
                                  << "scheduled:" << entry.departure.toString(Qt::ISODate)
                                  << "cancelled:" << entry.cancelled;
                break;
            }
        }
    }
    if (xml.hasError())
        qWarning().noquote() << "[DepartureTable] Changes XML error:" << xml.errorString();
    qInfo() << "[DepartureTable] Matched timetable changes:" << matchedChanges;
    return !xml.hasError();
}

void DepartureTableView::publishDepartures() {
    const QDateTime now = QDateTime::currentDateTime();
    int pastDepartures = 0;
    std::sort(m_timetableList.begin(), m_timetableList.end(), [](const TimetableEntry &left, const TimetableEntry &right) {
        return left.departure < right.departure;
    });

    m_departures.clear();
    for (const TimetableEntry &entry : m_timetableList) {
        if (entry.departure < now) {
            ++pastDepartures;
            continue;
        }
        QVariantMap departure;
        departure.insert("time", entry.departure.toString("HH:mm"));
        departure.insert("delayMinutes", entry.departure.secsTo(entry.liveDeparture) / 60);
        departure.insert("cancelled", entry.cancelled);
        departure.insert("line", entry.line);
        departure.insert("destination", entry.destination);
        departure.insert("trainNumber", entry.trainNumber);
        m_departures.append(departure);
        if (m_departures.size() == 5)
            break;
    }
        qInfo() << "[DepartureTable] Total parsed:" << m_timetableList.size()
            << "Past departures skipped:" << pastDepartures
            << "Upcoming departures published:" << m_departures.size();
    emit departuresChanged();
    setStatus(m_departures.isEmpty() ? "No upcoming departures" : QString());
}