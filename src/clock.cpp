#include "clock.h"

Clock::Clock(QObject *parent) : QObject(parent)
{
    updateTime();

    connect(&m_updateTimer, &QTimer::timeout, this, &Clock::updateTime);
    m_updateTimer.start(1000); // update every second

}


void Clock::updateTime() {
    QString newTime = QTime::currentTime().toString("hh:mm");
    if (newTime != this->m_timeString) {
        this->m_timeString = newTime;
        emit timeStringChanged();
    }
    return;
}
