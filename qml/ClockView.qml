import QtQuick 2.0
import MyApp 1.0

Rectangle {
   id: clockView
   anchors.horizontalCenter: parent.horizontalCenter
   anchors.verticalCenter: parent.verticalCenter
   //y: 0.5 * parent.height
   width: timeText.implicitWidth + 20
   height: timeText.implicitHeight + 20
   color: "transparent"

   Clock {
    id: digitalClock
   }

   Text {
        id: timeText
        anchors.centerIn: parent
        font.pointSize: 48
      color: "#25d9ff"
        text: digitalClock.timeString
   }
}

