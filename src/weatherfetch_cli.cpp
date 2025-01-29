
#include "WeatherSettings.h"
#include "WeatherParser.h"
#include "UrlFetcher.h"

#include <QCoreApplication>
#include <QUrl>
#include <cstdlib>
#include <functional>

int main(int argc, char *argv[])
{
    using namespace std::placeholders;
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("weatherfetch_cli");
    QCoreApplication::setApplicationVersion("1.0");
    WeatherSettings ws;
    if (argc >= 4) {
        // looks like we may have been passed a new location
        int i{0};
        float lat{static_cast<float>(std::atof(argv[++i]))};
        float lng{static_cast<float>(std::atof(argv[++i]))};
        QString cityname{};
        for (++i; i < argc; ++i) {
            cityname += argv[i];
            if (i != argc-1) {
                cityname += " ";
            }
        }
        ws.addLocation(lat, lng, cityname);
        ws.update();
    }
    UrlFetcher fetcher;
    WeatherParser parser;
    QObject::connect(&fetcher,
                     &UrlFetcher::receivedData,
                     &parser,
                     std::bind(&WeatherParser::update, &parser, ws.getCityName(), _1),
                     Qt::DirectConnection);
    QObject::connect(&parser, &WeatherParser::done, &app, &QCoreApplication::quit, Qt::DirectConnection);
    auto url{parser.createUrl(ws.getCityName(), ws.getCityLatitude(), ws.getCityLongitude(), ws.getApikey())};
    qInfo() << "Setting url to " << url;
    fetcher.fetch(url);
    return app.exec();
}

