
#include "WeatherSettings.h"
#include "WeatherParser.h"
#include "UrlFetcher.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QUrl>
#include <cstdlib>
#include <functional>

int main(int argc, char *argv[])
{
    using namespace std::placeholders;
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("weatherfetch_cli");
    QCoreApplication::setApplicationVersion("2.0");
    QCommandLineParser opts;
    opts.addHelpOption();
    opts.addVersionOption();
    opts.addOptions({
        {{"a", "add"},
            QCoreApplication::translate("main", "add location to top of list (TOL)."),
            QCoreApplication::translate("main", "lat,lng,cityname")
        },
        {{"c", "config"},
            QCoreApplication::translate("main", "manipulate the list but don't fetch weather.")},
        {{"d", "delete"},
            QCoreApplication::translate("main", "delete bottom of list (BOL).")},
        {{"l", "list"},
            QCoreApplication::translate("main", "list all current locations.")},
        {{"r", "rotate"},
            QCoreApplication::translate("main", "rotate the list to put TOL at BOL.")},
    });
    WeatherSettings ws;
    opts.process(app);
    if (opts.isSet("rotate")) {
        ws.rotate();
    }
    if (opts.isSet("delete")) {
        ws.removeLast();
    }
    if (opts.isSet("add")) {
        QString location = opts.value("add");
        QString lat = location.section(u',',0,0);
        QString lng = location.section(u',',1,1);
        QString cityname = location.section(u',',2);
        ws.addLocation(lat.toFloat(), lng.toFloat(), cityname);
    }
    if (opts.isSet("list")) {
        std::cout << ws << "\n";
    }
    bool fetch = !opts.isSet("config");
    ws.update();
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
    if (fetch) {
        fetcher.fetch(url);
    } else {
        // immediate quit if we're in config mode
        QTimer::singleShot(0, &app, &QCoreApplication::quit);
    }
    return app.exec();
}

