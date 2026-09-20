import QtQuick 2.0
import QtQuick.Layouts 1.0
import Weather 1.0

//Column Element containing all weatherView data when current data is present
ColumnLayout {
    property var weatherDataBackend
    anchors.centerIn: parent
    width: parent.width * 0.95
    height: parent.height * 0.95

    property bool daytime: false
    property color primaryTextColor: daytime ? "#ffd47a" : "#b8c7ff"
    property color secondaryTextColor: daytime ? "#8fc8c2" : "#9ca9d6"

    component TemperatureText: Text {
        id: temperatureDisplayroot

        // Custom properties
        property real value: 0
        property string unit: "°C"
        property int fontSize: 20

        // Display logic
        text: value + " " + unit

        // Styling
        font.pixelSize: fontSize
        color: primaryTextColor
    }

    //Row Divider between top and bottom
    RowLayout {
        Layout.fillHeight: true
        Layout.fillWidth: true

        // Left Element
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            radius: 20
            color: "transparent"
            Image {
                //source: "weatherIcons/clear-day.svg"
                source:
                {
                    let daytimeString = daytime ? "day" : "night"
                    let prePath = "qrc:/qml/weatherIcons/"
                    let fileType = ".svg"
                    switch (weatherBackend.weatherType) {
                    case WeatherViewBackend.Clear:
                        return prePath + "clear-" + daytimeString + fileType
                    case WeatherViewBackend.Cloudy:
                        return prePath + "cloudy-" + daytimeString + fileType
                    case WeatherViewBackend.Windy:
                        return prePath + "wind" + fileType
                    case WeatherViewBackend.Showers:
                        return prePath + "showers-" + daytimeString + fileType
                    case WeatherViewBackend.Rainy:
                        return prePath + "rainy" + fileType
                    case WeatherViewBackend.Snowy:
                        return prePath + "snowy" + fileType
                    case WeatherViewBackend.RainAndSnowMix:
                        return prePath + "rain-and-snow-mix" + fileType
                    case WeatherViewBackend.Hail:
                        return prePath + "hail" + fileType
                    case WeatherViewBackend.IsolatedThunderstorms:
                        return prePath + "isolated-thunderstorms-" + daytimeString + fileType
                    case WeatherViewBackend.Unspecified:
                        return ""
                    }
                }
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
            }
        }

        // Right Element
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            radius: 20

            ColumnLayout {
                anchors.fill: parent

                //Top Element in Current Temperature and Text
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "transparent"
                    TemperatureText {
                        anchors.centerIn: parent
                        value: weatherBackend.currentTemperature
                        fontSize: 20
                    }
                }
                Rectangle {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        width: parent.width
                        text: weatherBackend.currentWeatherString
                        font.pixelSize: 15
                        color: secondaryTextColor
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

    }

    //Bottom Element containing History data
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        radius: 20

        GridLayout {
            anchors.fill: parent
            columns: 2
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "HIGH:"
                    font.pixelSize: 20
                    color: secondaryTextColor
                }
            }
            Rectangle {
                color: "transparent"
                Layout.fillHeight: true
                Layout.fillWidth: true
                TemperatureText {
                    anchors.centerIn: parent
                    value: weatherBackend.maxTemperature
                    fontSize: 20
                }
            }
            Rectangle {
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "transparent"
                Text {
                    anchors.centerIn: parent
                    text: "LOW:"
                    font.pixelSize: 20
                    color: secondaryTextColor
                }
            }
            Rectangle {
                color: "transparent"
                Layout.fillHeight: true
                Layout.fillWidth: true
                TemperatureText {
                    anchors.centerIn: parent
                    value: weatherBackend.minTemperature
                    fontSize: 20
                }
            }
        }

        color: "transparent"
    }
}

