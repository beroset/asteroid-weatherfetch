/*
 * Copyright (C) 2022 - Ed Beroset <beroset@ieee.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <giomm.h>
#include "WeatherParser.h"
#include <QDebug>
#include <vector>

struct DayData {
    double mintemp;
    double maxtemp;
    int icon;
};

/*
 * This file has implementations for two different weather data providers:
 *      open-meteo
 *      openweathermap
 *
 *  Originally, the openweathermap application was used, but version 2.5
 *  of that API will no longer work after June 2024, so now we use the
 *  open-meteo API instead.
 *
 *  Both implementations are left here to serve as an illustration for how
 *  to change weather data providers, should that be necessary again.
 *
 *  To port to a new provider, one must implement only these two functions:
 *      QString WeatherParser::createUrl(QString cityName, QString lat, QString lon, QString apikey) const
 *      [[nodiscard]] static std::vector<DayData> weatherJsonToVector(const QString &weatherJson)
 *
 * The createUrl() function is a member function, callable from QML, that creates
 * and returns the URL given the passed parameters.  It is expected that some
 * other code will actually fetch the weather data using that URL.
 *
 * The second function parses the received weather data into a form that
 * is usable for the asteroid-weather app.
 *
 */
#define OPEN_METEO
#ifdef OPEN_METEO
/*!
 * look up weather icon code, given WMO weather code
 *
 * sources:
 * WMO weather codes:  https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
 * weather icon codes: https://openweathermap.org/weather-conditions
 */
[[nodiscard]] static int iconlookup(int wxcode) {
    int iconcode{800};  // every day is sunny!
    switch(wxcode) {
        case  0: iconcode = 800; break; // sunny
        case  1: iconcode = 801; break; // mainly sunny
        case  2: iconcode = 802; break; // partly cloudy
        case  3: iconcode = 803; break; // mostly cloudy
        case 45: iconcode = 741; break; // foggy
        case 48: iconcode = 741; break; // rime fog
        case 51: iconcode = 300; break; // light drizzle
        case 53: iconcode = 301; break; // drizzle
        case 55: iconcode = 302; break; // heavy drizzle
        case 56: iconcode = 612; break; // light freezing drizzle
        case 57: iconcode = 613; break; // freezing drizzle
        case 61: iconcode = 500; break; // light rain
        case 63: iconcode = 501; break; // rain
        case 65: iconcode = 502; break; // heavy rain
        case 66: iconcode = 511; break; // light freezing rain
        case 67: iconcode = 511; break; // freezing rain
        case 71: iconcode = 600; break; // light snow
        case 73: iconcode = 601; break; // snow
        case 75: iconcode = 602; break; // heavy snow
        case 77: iconcode = 601; break; // snow grains
        case 80: iconcode = 500; break; // light showers
        case 81: iconcode = 501; break; // showers
        case 82: iconcode = 502; break; // heavy showers
        case 85: iconcode = 600; break; // light snow showers
        case 86: iconcode = 601; break; // snow showers
        case 95: iconcode = 211; break; // thunderstorm
        case 96: iconcode = 200; break; // light thunderstorms with hail (no hail designation in codes, so just use t'storm)
        case 99: iconcode = 211; break; // thunderstorm with hail (no hail designation in codes, so just use t'storm)

        default: {
            iconcode = 800;
            qDebug() << "unknown weather code passed to iconlookup:" << wxcode;
            break;
        }
    }
    return iconcode;
}
/*!
 * \brief Convert JSON weather string to settings for asteroid-weather
 *
 * \param weatherJson String containing weather JSON.  As and example,
 * if we request the weather data for Cape Town, South Africa, the request
 * URL would be
 * "https://api.open-meteo.com/v1/forecast?latitude=35.858&longitude=-79.1032&timezone=auto&daily=weather_code,temperature_2m_max,temperature_2m_min"
 * and the response might be:
 *   "latitude":35.850216,"longitude":-79.097015,"generationtime_ms":0.102996826171875,"utc_offset_seconds":-14400,"timezone":"America/New_York","timezone_abbreviation":"EDT","elevation":188.0,"daily_units":{"time":"iso8601","weather_code":"wmo code","temperature_2m_max":"°C","temperature_2m_min":"°C"},"daily":{"time":["2024-04-25","2024-04-26","2024-04-27","2024-04-28","2024-04-29","2024-04-30","2024-05-01"],"weather_code":[3,3,3,3,2,51,2],"temperature_2m_max":[23.2,21.8,20.2,25.9,27.7,28.8,29.6],"temperature_2m_min":[8.8,8.6,11.5,11.2,13.3,14.9,14.2]}}
 *
 * Note that by default, the temperatures are in degrees C which we must convert to Kelvin for the weather app
 *  see https://open-meteo.com/
 */
[[nodiscard]] static std::vector<DayData> weatherJsonToVector(const QString &weatherJson)
{
    std::vector<DayData> days;
    constexpr double CtoKwithRounding{272.15 + 0.5};
    QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(weatherJson.toUtf8(), &parseError);
    if (json.isNull()) {
        qWarning() << "JSON parse error:  " << parseError.errorString();
        return days;
    }
    auto min = json["daily"]["temperature_2m_min"].toArray();
    auto max = json["daily"]["temperature_2m_max"].toArray();
    auto icon = json["daily"]["weather_code"].toArray();
    for (int i{0}; i < min.count(); ++i) {
        days.push_back({ min[i].toDouble() + CtoKwithRounding,
                         max[i].toDouble() + CtoKwithRounding,
                         iconlookup(icon[i].toInt())
                       });
    }
    return days;
}

QString WeatherParser::createUrl(QString cityName, QString lat, QString lon, QString apikey) const
{
    QString url = "https://api.open-meteo.com/v1/forecast"
        "?latitude=" + lat
        + "&longitude=" + lon
        + "&timezone=auto"
        + "&daily=weather_code,temperature_2m_max,temperature_2m_min";
    return url;
}
#else
/*!
 * \brief Convert JSON weather string to settings for asteroid-weather
 *
 * \param weatherJson String containing weather JSON.  An example of the
 * minimum acceptable string:
 *   '{"daily":[
 *      {"temp":{"min":289.19,"max":298.9}},{"weather":[{"id":800}]},
 *      {"temp":{"min":290.25,"max":300.2}},{"weather":[{"id":800}]}
 *    ]}'
 *  see https://openweathermap.org/api/one-call-api for full spec
 */
[[nodiscard]] static std::vector<DayData> weatherJsonToVector(const QString &weatherJson)
{
    std::vector<DayData> days;
    QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(weatherJson.toUtf8(), &parseError);
    if (json.isNull()) {
        qWarning() << "JSON parse error:  " << parseError.errorString();
        return days;
    }
    auto daily = json["daily"].toArray();
    for (const auto& day : daily) {
        days.push_back({ day.toObject()["temp"].toObject()["min"].toDouble(),
                         day.toObject()["temp"].toObject()["max"].toDouble(),
                         day.toObject()["weather"].toArray()[0].toObject()["id"].toInt()
                       });
    }
    return days;
}

QString WeatherParser::createUrl(QString cityName, QString lat, QString lon, QString apikey) const
{
    static const QString omit = "current,minutely,hourly,alerts";
    if (apikey == "" || cityName == "" || lat == "" || lon == "") {
        return "";
    }
    QString url = "https://api.openweathermap.org/data/2.5/onecall"
        "?lat=" + lat
        + "&lon=" + lon
        + "&exclude=" + omit
        + "&appid=" + apikey;
    return url;
}
#endif

void WeatherParser::setCityName(const QString &cityName)
{
    const Glib::RefPtr<Gio::Settings> settings = Gio::Settings::create("org.asteroidos.weather");
    settings->set_string("city-name", cityName.toUtf8().data());
}

void WeatherParser::parseWeatherJson(const QString &weatherJson)
{
    constexpr unsigned maxWeatherDays{5};
    auto days{weatherJsonToVector(weatherJson)};
    unsigned count{std::min(maxWeatherDays, days.size())};
    for (unsigned i = 0; i < count; ++i) {
        const Glib::RefPtr<Gio::Settings> settings = Gio::Settings::create("org.asteroidos.weather.day" + std::to_string(i));
        settings->set_int("min-temp", days[i].mintemp);
        settings->set_int("max-temp", days[i].maxtemp);
        settings->set_int("id", days[i].icon);
    }
    const Glib::RefPtr<Gio::Settings> settings = Gio::Settings::create("org.asteroidos.weather");
    settings->set_int("timestamp-day0", (int)time(NULL));
}

WeatherParser::WeatherParser()
{
    Glib::init();
    Gio::init();
}

void WeatherParser::updateWeather(QString cityname, QString weatherJson)
{
    qDebug() << "uw Cityname:" << cityname;
    setCityName(cityname);
    qDebug() << "uw Json weather string:" << weatherJson;
    parseWeatherJson(weatherJson);
}

void WeatherParser::update(QString cityname, QString weatherJson)
{
    using namespace std::chrono_literals;
    if (cityname == nullptr || weatherJson == nullptr) {
        qDebug() << "Error: arguments to WeatherParser::update were null";
        emit exit(1);
    }
    updateWeather(cityname, weatherJson);
    g_settings_sync();
    emit done();
}
