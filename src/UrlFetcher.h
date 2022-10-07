#ifndef FETCHER_H
#define FETCHER_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>

class UrlFetcher : public QObject
{
    Q_OBJECT
public:
    explicit UrlFetcher(QObject *parent = nullptr);
    void fetch(QUrl url);

signals:
    void receivedData(QString data);
    void done();

public slots:
    void replyFinished(QNetworkReply* reply);
};
#endif // FETCHER_H
