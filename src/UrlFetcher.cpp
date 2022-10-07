#include "UrlFetcher.h"
#include <QString>
#include <QNetworkAccessManager>
#include <iostream>

UrlFetcher::UrlFetcher(QObject *parent) : QObject(parent)
{}

void UrlFetcher::fetch(QUrl url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,
            this, &UrlFetcher::replyFinished);
    manager->get(QNetworkRequest(url));
}

void UrlFetcher::replyFinished(QNetworkReply *reply) {
    QString str = reply->readAll();
    emit receivedData(str);
    emit done();
}

