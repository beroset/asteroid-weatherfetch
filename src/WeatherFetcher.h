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

#ifndef WEATHERFETCHER_H
#define WEATHERFETCHER_H

#include <QObject>
#include <QString>
#include <QJsonArray>

class WeatherFetcher : public QObject
{
    Q_OBJECT

public:
    explicit WeatherFetcher(QObject *parent = nullptr);
    ~WeatherFetcher();
    QString getApikey() const;
    QString getCityName() const;
    QString getCityLatitude() const;
    QString getCityLongitude() const;

signals:
    void update(QString* cityname, QString* weatherJson);

public slots:
    void receivedData(QString data);

private:
    QString apikey;
    QJsonArray locations;
    QString cityName;
    QString weatherData;
};

#endif

