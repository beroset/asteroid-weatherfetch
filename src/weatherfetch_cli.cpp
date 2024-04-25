
#include "WeatherSettings.h"
#include "WeatherParser.h"
#include "UrlFetcher.h" 

#include <QCoreApplication>
#include <QUrl>
#include <functional>

int main(int argc, char *argv[])
{
    using namespace std::placeholders;
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("weatherfetch_cli");
    QCoreApplication::setApplicationVersion("1.0");
    WeatherSettings ws;
    UrlFetcher fetcher;
    WeatherParser parser;
    QObject::connect(&fetcher, &UrlFetcher::receivedData, &parser, &WeatherParser::receivedData, Qt::DirectConnection);
    QObject::connect(&parser, &WeatherParser::done, &app, &QCoreApplication::quit, Qt::DirectConnection);
    auto url{parser.createUrl(ws.getCityName(), ws.getCityLatitude(), ws.getCityLongitude(), ws.getApikey())};
    qInfo() << "Setting url to " << url;
    fetcher.fetch(url);
    return app.exec();
}

