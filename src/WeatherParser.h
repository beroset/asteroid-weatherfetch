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

#ifndef WEATHERPARSER_H
#define WEATHERPARSER_H

#include <QObject>
#include <QtQml>

class Q_DECL_EXPORT WeatherParser : public QObject
{
    Q_OBJECT

public:
    WeatherParser();
    Q_INVOKABLE void updateWeather(QString cityname, QString weatherJson);

public slots:
    void update(QString* cityname, QString* weatherJson);

signals:
    void done();

private:
    void setCityName(const QString &cityName);
    void parseWeatherJson(const QString &weatherJson);
};

#endif

