#include "calendarView.h"

constexpr uint REFRESHRATE = 3600000; //Refreshes weather data every hour

CalendarViewBackend::CalendarViewBackend() {
    connect(&m_timer, &QTimer::timeout, this, &CalendarViewBackend::updateDateRefreshCheck);
    m_timer.start(REFRESHRATE);

    this->updateDateRefreshCheck();
}

void CalendarViewBackend::updateDateRefreshCheck() {
    if (this->currentDateDay != QDate::currentDate().day()) {
        this->currentDateDay = QDate::currentDate().day();
        emit updateCalendarDay();
    }
    if (this->currentMonth != QDate::currentDate().month()) {
        this->currentMonth = QDate::currentDate().month();
        emit updateCalendarMonth();
    }
}
