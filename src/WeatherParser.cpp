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

void WeatherParser::setCityName(const QString &cityName)
{
    const Glib::RefPtr<Gio::Settings> settings = Gio::Settings::create("org.asteroidos.weather");
    settings->set_string("city-name", cityName.toUtf8().data());
}

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
void WeatherParser::parseWeatherJson(const QString &weatherJson)
{
    constexpr int maxWeatherDays{5};
    QJsonParseError parseError;
    auto json = QJsonDocument::fromJson(weatherJson.toUtf8(), &parseError);
    if (json.isNull()) {
        qWarning() << "JSON parse error:  " << parseError.errorString();
    }
    auto daily = json["daily"].toArray();
    int count = std::min(maxWeatherDays, daily.count());
    for (int i = 0; i < count; ++i) {
        const Glib::RefPtr<Gio::Settings> settings = Gio::Settings::create("org.asteroidos.weather.day" + std::to_string(i));
        auto day = daily.at(i).toObject();
        short low = day["temp"].toObject()["min"].toDouble();
        settings->set_int("min-temp", low);
        short high = day["temp"].toObject()["max"].toDouble();
        settings->set_int("max-temp", high);
        short icon = day["weather"].toArray()[0].toObject()["id"].toInt();
        settings->set_int("id", icon);
    }
    const Glib::RefPtr<Gio::Settings> settings = Gio::Settings::create("org.asteroidos.weather");
    settings->set_int("timestamp-day0", (int)time(NULL));
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

void WeatherParser::update(QString* cityname, QString* weatherJson)
{
    using namespace std::chrono_literals;
    if (cityname == nullptr || weatherJson == nullptr) {
        qDebug() << "Error!  arguments to WeatherParser::update were null";
        emit done();
    }
    qDebug() << "Cityname: " << *cityname;
    setCityName(*cityname);
    qDebug() << "Json weather string: " << *weatherJson;
    parseWeatherJson(*weatherJson);
    g_settings_sync();
    emit done();
}

