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

#include "WeatherSettings.h"
#include "UrlFetcher.h"
#include <QSettings>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>
#include <format>

/* This needs to fetch the apikey, city name, lat and long from the asteroid-weather config
 * Then construct the url
 * Then fetch the data from the URL using URLFetcher
 * Then pass city name and fetched data to the WeatherParser
 */
QString WeatherSettings::getApikey() const
{
    return apikey;
}

QString WeatherSettings::getCityName() const
{
    return locations.first()["name"].toString();
}

QString WeatherSettings::getCityLatitude() const
{
    return locations.first()["lat"].toString();
}

QString WeatherSettings::getCityLongitude() const
{
    return locations.first()["lng"].toString();
}

WeatherSettings::WeatherSettings(QObject *parent) : QObject(parent)
{
    QSettings top("asteroid-weatherfetch", "asteroid-weatherfetch");
    top.setIniCodec("UTF-8");
    top.beginGroup("Weather");
    apikey = top.value("apikey", QString()).toString();
    auto inter = top.value("savedlocations", QString()).toString();
    auto jdoc = QJsonDocument::fromJson(inter.toUtf8());
    locations = jdoc.array();
    qDebug() << "filename = " << top.fileName();
    qDebug() << getCityName() << " ( " << getCityLatitude() << " , " << getCityLongitude() << " )";
    cityName = getCityName();
    top.endGroup();
}

WeatherSettings::~WeatherSettings()
{
    qDebug() << "destroying WeatherSettings";
}

void WeatherSettings::addLocation(float lat, float lng, QString name, bool atEnd) {
    QString latstring = QString::fromStdString(std::format("{:.4f}", lat));
    QString lngstring = QString::fromStdString(std::format("{:.4f}", lng));
    addLocation(latstring, lngstring, name, atEnd);
}

void WeatherSettings::addLocation(QString latstring, QString lngstring, QString name, bool atEnd)
{
    QJsonObject newloc{
        { "name", name },
        { "lat", latstring },
        { "lng", lngstring }
    };
    if (atEnd) {
        locations.push_back(newloc);
    } else {
        locations.push_front(newloc);
    }
}

void WeatherSettings::update()
{
    QSettings top("asteroid-weatherfetch", "asteroid-weatherfetch");
    top.setIniCodec("UTF-8");
    auto jdoc = QJsonDocument(locations);
    QString loc = QString::fromStdString(
        jdoc.toJson(QJsonDocument::Compact).toStdString()
    );
    top.setValue("Weather/savedlocations", loc);
}

void WeatherSettings::rotate()
{
    addLocation(getCityLatitude(), getCityLongitude(), getCityName(), true);
    locations.removeFirst();
}

void WeatherSettings::removeLast()
{
    locations.removeLast();
}

std::ostream& operator<<(std::ostream& out, const WeatherSettings& ws) 
{
    int i{0};
    for (int i{0}; i < ws.locations.size(); ++i) {
        out << ws.locations.at(i)["lat"].toString().toStdString()
            << "," << ws.locations.at(i)["lng"].toString().toStdString()
            << ",\"" << ws.locations.at(i)["name"].toString().toStdString()
            << "\"\n";
    }
    return out;
}
