#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "clock.h"
#include "weatherView.h"
#include "calendarView.h"
#include "calendarEvents.h"
#include "departureTableView.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    qmlRegisterType<Clock>("MyApp", 1, 0, "Clock");
    qmlRegisterType<CalendarViewBackend>("MyApp", 1, 0, "CalendarBackend");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QNetworkAccessManager *nam = new QNetworkAccessManager();
    WeatherViewBackend* weatherbackend = new WeatherViewBackend(nullptr, nam);
    QQmlContext* weatherContext = engine.rootContext();
    weatherContext->setContextProperty("weatherBackend", weatherbackend);
    qmlRegisterUncreatableType<WeatherViewBackend>(
        "Weather", 1, 0, "WeatherViewBackend",
        "Only used for enums"
    );
    CalendarEventsBackend* calendarEventsBackend = new CalendarEventsBackend(nullptr, nam);
    QQmlContext* calendarEventsContext = engine.rootContext();
    calendarEventsContext->setContextProperty("calendarEventsBackend", calendarEventsBackend);
    qmlRegisterUncreatableType<CalendarEventsBackend>(
        "CalendarEvents", 1, 0, "CalendarEventsBackend",
        "Only used for enums"
    );

    // ## Departure Table
    DepartureTableView* departureTableBackend = new DepartureTableView(nullptr, nam);
    QQmlContext* departureViewContext = engine.rootContext();
    departureViewContext->setContextProperty("departureTableBackend", departureTableBackend);

    engine.load(url);
    return app.exec();
}
