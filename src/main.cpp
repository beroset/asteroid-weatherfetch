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
 */

#include <QGuiApplication>
#include <QQuickView>
#include <QScreen>
#include <QtQml>

#include <asteroidapp.h>

#include "weatherparser.h"
#if 0
#include "tilttowake.h"
#include "taptowake.h"
#endif

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(AsteroidApp::application(argc, argv));
    QScopedPointer<QQuickView> view(AsteroidApp::createView());
    qmlRegisterType<WeatherParser>("org.asteroid.weatherfetch", 1, 0, "WeatherParser");
#if 0
    qmlRegisterType<VolumeControl>("org.asteroid.settings", 1, 0, "VolumeControl");
    qmlRegisterType<TapToWake>("org.asteroid.settings", 1, 0, "TapToWake");
#endif
    view->setSource(QUrl("qrc:/qml/main.qml"));
#if 0
    view->rootContext()->setContextProperty("qtVersion", QString(qVersion()));
    view->rootContext()->setContextProperty("kernelVersion", QString(buf.release));
#endif
    view->resize(app->primaryScreen()->size());
    view->show();
    return app->exec();
}
