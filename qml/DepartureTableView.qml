import QtQuick 2.0
import QtQuick.Layouts

Item {
    anchors.fill: parent

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width * 0.9
        height: parent.height * 0.9

        Text {
            Layout.fillWidth: true
            text: "Departures"
            color: "#25d9ff"
            font.bold: true
            font.pixelSize: 20
            horizontalAlignment: Text.AlignHCenter
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: departureTableBackend.departures
            spacing: 4
            delegate: RowLayout {
                width: ListView.view.width
                Text { text: modelData.time; color: "#d8f7ff"; Layout.preferredWidth: 55; font.bold: true }
                Text { text: modelData.line; color: "#25d9ff"; Layout.preferredWidth: 45; font.bold: true }
                Text { text: modelData.destination || "Unknown destination"; color: "#d8f7ff"; Layout.fillWidth: true; elide: Text.ElideRight }
                Text {
                    text: modelData.cancelled ? "Train cancelled" : (modelData.delayMinutes > 0 ? "(+" + modelData.delayMinutes + " min)" : "")
                    color: modelData.cancelled || modelData.delayMinutes > 0 ? "#ff6b6b" : "#7fb4c2"
                    Layout.preferredWidth: 105
                    horizontalAlignment: Text.AlignRight
                }
            }
        }
        Text {
            Layout.fillWidth: true
            text: departureTableBackend.status
            color: "#7fb4c2"
            horizontalAlignment: Text.AlignHCenter
            visible: text.length > 0
        }

    }
}
