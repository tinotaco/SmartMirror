#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include <QObject>
#include <QTimer>
#include <QDate>

class CalendarViewBackend: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString dateString READ getDateString NOTIFY updateCalendarDay)
    Q_PROPERTY(int currentDay READ getCurrentMonthDay NOTIFY updateCalendarDay)
    Q_PROPERTY(int maxMonthDays READ getMaxMonthDay NOTIFY updateCalendarMonth)
    Q_PROPERTY(int monthWeekdayOffset READ getMonthWeekdayOffset NOTIFY updateCalendarMonth)


    public:
        explicit CalendarViewBackend();

    QString getDateString() {return QDate::currentDate().toString("dddd MMM d, yyyy");};
    int getMaxMonthDay() {return QDate::currentDate().daysInMonth();};
    int getCurrentMonthDay() {return this->currentDateDay;};
    int getMonthWeekdayOffset() {
        QDate now = QDate::currentDate();
        return QDate(now.year(), now.month(), 1).dayOfWeek() - 1; //Gets day of week of first day in current month and convert to offset range 0-6
    };
    void updateDateRefreshCheck(); //Check to see if day of month has changed

   private:
        QTimer m_timer;
        int currentDateDay; //Current day of month
        int currentMonth; //Current Month value to compare if month


    signals:
        void updateCalendarDay();
        void updateCalendarMonth();

};


#endif // CALENDARVIEW_H
