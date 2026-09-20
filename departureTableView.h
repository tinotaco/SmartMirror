#ifndef DEPARTURETABLEVIEW_H
#define DEPARTURETABLEVIEW_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QVariantList>
#include <QTimer>
#include <QDateTime>

class DepartureTableView : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList departures READ departures NOTIFY departuresChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    DepartureTableView(QObject *parent = nullptr, QNetworkAccessManager* nam = nullptr);
    QVariantList departures() const;
    QString status() const;

signals:
    void departuresChanged();
    void statusChanged();


private:
    QNetworkAccessManager* m_nam;
    struct TimetableEntry {
        QString serviceId;
        QDateTime departure;
        QDateTime liveDeparture;
        QString line;
        QString destination;
        QString trainNumber;
        bool cancelled = false;
    };

    QTimer m_refreshTimer;
    QList<TimetableEntry> m_timetableList;
    QString m_evaNumber;
    QString m_clientId;
    QString m_apiKey;
    QVariantList m_departures;
    QString m_status;
    int m_pendingRequests = 0;
    bool m_refreshInProgress = false;

    void requestDepartures();
    void requestPlan(const QDateTime &hour);
    void requestChanges(const QString &url, const QString &source);
    bool parsePlanXML(const QByteArray &data);
    bool parseChangesXML(const QByteArray &data, const QString &source);
    bool loadConfig();
    void setStatus(const QString &status);
    void publishDepartures();

};


#endif // DEPARTURETABLEVIEW_H
