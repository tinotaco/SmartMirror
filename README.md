# SmartMirror

SmartMirror is a Qt Quick desktop application for displaying useful information on a home mirror dashboard. It currently includes:

- Digital clock
- Weather conditions and temperature history
- Calendar month view and upcoming events
- Deutsche Bahn departures, including delays and cancellations

## Requirements

- Qt 6 with the `Core`, `Quick`, `Network`, and `NetworkAuth` modules
- CMake 3.14 or newer
- A C++11-compatible compiler
- Internet access for weather, calendar, and Deutsche Bahn data

The project has been developed and tested with Qt 6.11 on macOS.

## Configuration

Create a local configuration file from the example:

```sh
cp config.ini.example config.ini
```

Then fill in the credentials:

```ini
[station]
evaNumber=8002161

[db]
clientId=YOUR_DB_CLIENT_ID
apiKey=YOUR_DB_API_KEY

[calendar]
clientId=YOUR_GOOGLE_OAUTH_CLIENT_ID
clientSecret=YOUR_GOOGLE_OAUTH_CLIENT_SECRET
summary=Your calendar name

[weather]
apiKey=YOUR_WEATHER_API_KEY
latitude=48.026500
longitude=11.589800
```

The EVA number identifies the station used for departures. `8002161` is Furth(b Deisenhofen).

The calendar uses Google OAuth. The application does not store or request a Google username and password. When authentication is required, Google opens a browser login flow.

Keep `config.ini` private. It contains credentials and is ignored by Git. Do not commit it or share its contents.

## Build

From the project directory:

```sh
cmake -S . -B build/current-debug -DCMAKE_PREFIX_PATH=/path/to/Qt/6.11.0/macos
cmake --build build/current-debug --parallel 4
```

Replace `/path/to/Qt/6.11.0/macos` with the location of your Qt installation. For example:

```sh
cmake -S . -B build/current-debug -DCMAKE_PREFIX_PATH="$HOME/Qt/6.11.0/macos"
```

## Run

```sh
./build/current-debug/SmartMirror
```

The application searches for `config.ini` beside the executable, in the current working directory, and then in the platform application configuration directory.

## Deutsche Bahn departures

The departure table requests the current and next timetable hour and shows up to five upcoming trains. It also requests planned and recent changes to calculate delays and identify cancellations. Diagnostic messages are printed to the terminal with the prefix:

```text
[DepartureTable]
```

These messages include request URLs, HTTP status information, parsed train IDs, scheduled times, live times, and filtering results. API credentials are not printed.

## Project structure

- `main.cpp` - application startup and QML context setup
- `main.qml` - dashboard layout
- `departureTableView.cpp/.h` - Deutsche Bahn requests and departure model
- `DepartureTableView.qml` - departure table presentation
- `weatherView.cpp/.h` - weather API integration
- `WeatherView.qml` and `WeatherDataView.qml` - weather presentation
- `calendarEvents.cpp/.h` - Google Calendar OAuth and event retrieval
- `CalendarView.qml` and `CalendarEvents.qml` - calendar presentation
- `qml.qrc` - QML and weather icon resources
