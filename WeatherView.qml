import QtQuick 2.0
import QtQuick.Layouts 1.15
import MyApp 1.0

Item {
    //Main Placeholder Element for the Weather View
    id: weatherView
    anchors.centerIn: parent
    property color backgroundColor: !weatherBackend.getConnectionState ? "#183044" :
                                           weatherBackend.isDaytime ? "#28657e" : "#202b5b"
    width: parent.width * 0.9
    height: parent.height * 0.9


    //Create connection to backend weatherBackend which pulls and updates info from API
    Connections {
        target: weatherBackend
    }

    Rectangle {
        anchors.fill: parent
        radius: 16
        color: backgroundColor
        border.color: weatherBackend.isDaytime ? "#79e7ff" : "#8298ff"
        border.width: 1
        opacity: 0.9
    }

    //Text Element visible if there is no Connection
    Text {
        id: weatherNotConnected
        visible:
        {
            !weatherBackend.getConnectionState
        }
        anchors.centerIn: parent
        text: "Not Connected"
        color: "#d8f7ff"
    }


    WeatherDataView {
        id: rootWeatherDataView
        daytime: weatherBackend.isDaytime
        visible: weatherBackend.getConnectionState
        weatherDataBackend: weatherBackend
    }
}
