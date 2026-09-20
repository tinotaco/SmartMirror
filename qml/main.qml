import QtQuick
import QtQuick.Window

Window {
    width: 720
    height: 960
    visible: true
    title: qsTr("Hello World")

    property int borderWidth: 1
    property int radiusSize: 20
    property string borderColor: "#67e8ff"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#071a2b" }
            GradientStop { position: 0.55; color: "#0d3448" }
            GradientStop { position: 1.0; color: "#124a5a" }
        }
    }

    Rectangle {
        id: rootClockRectangle
        width: 0.42 * parent.width
        height: 0.1 * parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        y: 0.10 * parent.height
        color: "#0b2537"
        border.color: borderColor
        border.width: borderWidth
        radius: 20

        ClockView {
            id: externalClock
        }
    }

    Rectangle {
        id: weatherItem
        width: 0.4 * parent.width
        height: 0.2 * parent.height
        radius: radiusSize
        border.color: borderColor
        border.width: borderWidth
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: - parent.width / 4
        y: 0.3 * parent.height
        color: "#0b2b40"

        WeatherView {
        }
    }

    Rectangle {
        id: calendarRectangle
        radius: radiusSize
        border.color: borderColor
        border.width: borderWidth
        width: 0.4 * parent.width
        height: 0.2 * parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: parent.width / 4
        y: 0.35 * parent.height
        color: "#0b2537"
        CalendarView {}
    }

    Rectangle {
        id: departureTable
        radius: radiusSize
        border.color: borderColor
        border.width: borderWidth
        width: 0.5 * parent.width
        height: 0.15 * parent.height
        anchors.left: weatherItem.left
        y: 0.6 * parent.height
        color: "#0b2537"
        DepartureTableView{}
    }

    Rectangle {
        id: calendarEventsRectangle
        radius: radiusSize
        border.color: borderColor
        border.width: borderWidth
        width: 0.45 * parent.width
        height: 0.12 * parent.height
        anchors.right: calendarRectangle.right
        y: 0.8 * parent.height
        color: "#0b2537"
        CalendarEvents {}
    }

}


