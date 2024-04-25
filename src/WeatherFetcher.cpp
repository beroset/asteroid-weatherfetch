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

#include "WeatherFetcher.h"
#include "WeatherParser.h"
#include "UrlFetcher.h"
#include <QSettings>
#include <QDebug>
#include <iostream>

/* This needs to fetch the apikey, city name, lat and long from the asteroid-weather config
 * Then construct the url
 * Then fetch the data from the URL using URLFetcher
 * Then pass city name and fetched data to the WeatherParser
 */
QString WeatherFetcher::getApikey() const
{
    return apikey;
}

QString WeatherFetcher::getCityName() const
{
    return locations.first()["name"].toString();
}

QString WeatherFetcher::getCityLatitude() const
{
    return locations.first()["lat"].toString();
}

QString WeatherFetcher::getCityLongitude() const
{
    return locations.first()["lng"].toString();
}

WeatherFetcher::WeatherFetcher(QObject *parent) : QObject(parent)
{
    QSettings top("asteroid-weatherfetch", "asteroid-weatherfetch");
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

WeatherFetcher::~WeatherFetcher()
{
    qDebug() << "destroying WeatherFetcher";
}

void WeatherFetcher::receivedData(QString data)
{
    weatherData = data;
    qInfo() << "WxData = " << weatherData;
    qInfo() << "CityName = " << cityName;
    emit update(&cityName, &weatherData);
}
