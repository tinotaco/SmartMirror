import QtQuick 2.0
import QtQuick.Layouts
import MyApp 1.0

Item {
    anchors.centerIn: parent
    width: parent.width * 0.9
    height: parent.height * 0.9

    CalendarBackend { id: calendarBackend }

    component WeekDayTitle: Rectangle {
        color: "transparent"
        Layout.fillHeight: true
        Layout.fillWidth: true
        property string textElement
        Text {
            anchors.centerIn: parent
            text: textElement
            color: "#7fb4c2"
        }
    }

    component MonthDay: Rectangle {
        property int maxDay: calendarBackend.maxMonthDays //This is the max day in the month variable. Controls if a field is visible or not
        property int elementMonthOffset: calendarBackend.monthWeekdayOffset //This is the offset weekday in which the month is started {0: Monday, 1: Tuesday, 2: Wednesday, and so on}
        property int monthListElement
        property int currentDay: calendarBackend.currentDay
        color: "transparent"
        Layout.fillWidth: true
        Layout.fillHeight: true
        property string dateday
        Rectangle {
            anchors.fill: parent
            color: "#25d9ff"
            radius: parent.width / 2
            visible: {
                console.log("We here in: " + monthListElement)

                if (monthListElement - elementMonthOffset == currentDay) {
                    return true
                } else {
                    return false
                }
            }
        }
        Text {
            anchors.centerIn: parent
            visible: {
                if (monthListElement - elementMonthOffset <= 0 || monthListElement - elementMonthOffset > maxDay) {
                    false
                } else {
                    true
                }
            }

            text: monthListElement - elementMonthOffset
            color: "#d8f7ff"
        }
    }

    ColumnLayout {
        anchors.fill: parent

        Rectangle {
            id: dateStringText
            Layout.preferredHeight: parent.height * 1/5
            Layout.fillWidth: true
            color: "transparent"
            Text {
                anchors.centerIn: parent
                text: calendarBackend.dateString
                font.pixelSize: 15
                font.bold: true
                color: "#25d9ff"
            }

        }

        GridLayout {
            id: root
            columns: 7
            Layout.preferredHeight: parent.height * 4/5
            Layout.fillWidth: true
            property int monthOffset: 0
            property int maxDay: calendarBackend.maxMonthDays

            WeekDayTitle { textElement: "M" }
            WeekDayTitle { textElement: "T" }
            WeekDayTitle { textElement: "W" }
            WeekDayTitle { textElement: "T" }
            WeekDayTitle { textElement: "F" }
            WeekDayTitle { textElement: "S" }
            WeekDayTitle { textElement: "S" }

            MonthDay { monthListElement: 1 }
            MonthDay { monthListElement: 2 }
            MonthDay { monthListElement: 3 }
            MonthDay { monthListElement: 4 }
            MonthDay { monthListElement: 5 }
            MonthDay { monthListElement: 6 }
            MonthDay { monthListElement: 7 }
            MonthDay { monthListElement: 8 }
            MonthDay { monthListElement: 9 }
            MonthDay { monthListElement: 10 }
            MonthDay { monthListElement: 11 }
            MonthDay { monthListElement: 12 }
            MonthDay { monthListElement: 13 }
            MonthDay { monthListElement: 14 }
            MonthDay { monthListElement: 15 }
            MonthDay { monthListElement: 16 }
            MonthDay { monthListElement: 17 }
            MonthDay { monthListElement: 18 }
            MonthDay { monthListElement: 19 }
            MonthDay { monthListElement: 20 }
            MonthDay { monthListElement: 21 }
            MonthDay { monthListElement: 22 }
            MonthDay { monthListElement: 23 }
            MonthDay { monthListElement: 24 }
            MonthDay { monthListElement: 25 }
            MonthDay { monthListElement: 26 }
            MonthDay { monthListElement: 27 }
            MonthDay { monthListElement: 28 }
            MonthDay { monthListElement: 29 }
            MonthDay { monthListElement: 30 }
            MonthDay { monthListElement: 31 }
            MonthDay { monthListElement: 32 }
            MonthDay { monthListElement: 33 }
            MonthDay { monthListElement: 34 }
            MonthDay { monthListElement: 35 }
        }
    }
}
