#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>
#include <QTimer>
#include <QTime>

class Clock : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString timeString READ getTimeString NOTIFY timeStringChanged)
public:
    explicit Clock(QObject *parent = nullptr);

    QString getTimeString() { return m_timeString; }

private:
    QTimer m_updateTimer;
    QString m_timeString;

    void updateTime();

signals:
    void timeStringChanged();
};

#endif // CLOCK_H
