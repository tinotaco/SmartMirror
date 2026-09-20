import QtQuick 2.0
import QtQuick.Layouts
import MyApp 1.0
import CalendarEvents 1.0

Item {
    id: root
    anchors.fill: parent
    property bool connected: true

    signal refreshCalendarEvents()

    Connections {
        target: calendarEventsBackend
    }

    component CalendarEvent : Rectangle {
        color: "transparent"

        property int elementIndex
        property string eventTimeString
        property string title

        Connections {
            target: calendarEventsBackend

            function onUpdateCalendarEvents() {
                refresh()
            }
        }


        function refresh() {
            console.log("We are updating calendarEvent data. Element:", elementIndex)
            let calendarEventEntry = calendarEventsBackend.returnCalendarEventEntry(elementIndex)
            console.log("Description String is: ", calendarEventEntry.descriptionString)

            switch (calendarEventEntry.eventType) {
            case CalendarEventsBackend.NOEVENT:
                title = ""
                eventTimeString = ""
                return;
            case CalendarEventsBackend.APPOINTMENT:
                title = "Appointment"
                eventTimeString = "hihi"
                title = calendarEventEntry.descriptionString
                eventTimeString = calendarEventEntry.startTime.toLocaleTimeString(Qt.locale(), "HH:mm")
                        + " - " + calendarEventEntry.endTime.toLocaleTimeString(Qt.locale(), "HH:mm")
                console.log("in Appointment: ", title, eventTimeString)
                return;
            case CalendarEventsBackend.ALLDAY:
                title = calendarEventEntry.descriptionString
                eventTimeString = "All - day"
                console.log("in ALLDAY: ", title, eventTimeString)
                return;
            case CalendarEventsBackend.UPCOMING:
                title = calendarEventEntry.descriptionString
                if (calendarEventEntry.daysUntil === 1) {
                    eventTimeString = "Morgen"
                } else {
                    eventTimeString = "in " + calendarEventEntry.daysUntil + " Days"
                }
                return;
            }
        }

        RowLayout {
            anchors.fill: parent
            Rectangle {
                color: "transparent"
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 1/3
                Text {
                    anchors.centerIn: parent
                    text: eventTimeString
                        color: "#7fb4c2"
                }
            }

            Rectangle {
                color: "transparent"
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 2/3
                Text {
                    anchors.centerIn: parent
                    text: title
                        color: "#d8f7ff"
                    //text: "Tino + Feli Calendar Event"
                }
            }
        }
    }

    ColumnLayout {
        height: parent.height
        width: parent.width - 45
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 2
        visible: connected
        Rectangle {
            id: calendarTitle
            Layout.preferredHeight: parent.height * 1/5
            Layout.fillWidth: true
            color: "transparent"
            Text {
                anchors.centerIn: parent
                text: "Calendar"
                font.pixelSize: 15
                font.bold: true
                color: "#25d9ff"
            }
        }

        ColumnLayout {
            id: calendarThreeEvents
            Layout.preferredHeight: parent.height * 4/5
            Layout.fillWidth: true

            CalendarEvent {Layout.fillWidth: true; Layout.fillHeight: true; elementIndex: 0}
            CalendarEvent {Layout.fillWidth: true; Layout.fillHeight: true; elementIndex: 1}
            CalendarEvent {Layout.fillWidth: true; Layout.fillHeight: true; elementIndex: 2}
        }
    }
}
