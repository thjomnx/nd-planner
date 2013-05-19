/*
*
* This file is part of QMapControl,
* an open-source cross-platform map widget
*
* Copyright (C) 2007 - 2008 Kai Winter
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QMapControl. If not, see <http://www.gnu.org/licenses/>.
*
* Contact e-mail: kaiwinter@gmx.de
* Program URL   : http://qmapcontrol.sourceforge.net/
*
*/

#include "mapnetwork.h"
#include <QWaitCondition>
namespace qmapcontrol
{
    MapNetwork::MapNetwork(ImageManager* parent)
        :parent(parent), manager(new QNetworkAccessManager(this)), requestId(0), loaded(0)
    {
        connect(this->manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
                this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));

        connect(this->manager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(requestFinished(QNetworkReply*)));
    }

    MapNetwork::~MapNetwork()
    {
        // TODO Cancel pending/running requests
        delete manager;
    }


    void MapNetwork::loadImage(const QString& host, const QString& url)
    {
        int id = requestId++;

        QNetworkRequest request;
        request.setUrl(QUrl("http://" + host + url));
        request.setRawHeader("User-Agent", "Mozilla");
        request.setAttribute(QNetworkRequest::User, id);

        qDebug() << "Starting request for " << request.url().toString();

        manager->get(request);

        if (vectorMutex.tryLock())
        {
            loadingMap[id] = url;
            vectorMutex.unlock();
        }
    }

    void MapNetwork::requestFinished(QNetworkReply* reply)
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Network error: " << reply->errorString();
        }
        else if (vectorMutex.tryLock())
        {
            QNetworkRequest request = reply->request();
            int id = request.attribute(QNetworkRequest::User).toInt();

            // Check if request is in map
            if (loadingMap.contains(id))
            {
                qDebug() << "Request finished for " << loadingMap[id];

                QString url = loadingMap[id];
                loadingMap.remove(id);
                vectorMutex.unlock();

                QByteArray ax;

                if (reply->bytesAvailable() > 0)
                {
                    QPixmap pm;
                    ax = reply->readAll();

                    if (pm.loadFromData(ax))
                    {
                        loaded += pm.size().width() * pm.size().height() * pm.depth() / 8 / 1024;
                        parent->receivedImage(pm, url);
                    }
                    else
                    {
                        qDebug() << "NETWORK_PIXMAP_ERROR: " << ax;
                    }
                }
            }
            else
            {
                vectorMutex.unlock();
            }

        }
        if (loadingMap.size() == 0)
        {
            parent->loadingQueueEmpty();
        }
    }

    void MapNetwork::abortLoading()
    {
        // TODO Cancel pending/running requests

        if (vectorMutex.tryLock())
        {
            loadingMap.clear();
            vectorMutex.unlock();
        }
    }

    bool MapNetwork::imageIsLoading(QString url)
    {
        return loadingMap.values().contains(url);
    }

    void MapNetwork::setProxy(QString host, int port)
    {
#ifndef Q_WS_QWS
        // Do not set proxy on Qt extended
        QNetworkProxy proxy;
        proxy.setHostName(host);
        proxy.setPort(port);

        manager->setProxy(proxy);
#endif
    }

    void MapNetwork::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
    {
        qDebug() << "Proxy Auth required" << proxy.hostName() << &authenticator;
        QDialog dialog;
        QGridLayout layout;
        QLabel username, password;
        username.setText("Username:");
        password.setText("Password:");
        layout.addWidget(&username, 0, 0);
        layout.addWidget(&password, 1, 0);
        QLineEdit user, pass;
        pass.setEchoMode(QLineEdit::Password);
        connect(&user, SIGNAL(returnPressed()), &dialog, SLOT(accept()));
        connect(&pass, SIGNAL(returnPressed()), &dialog, SLOT(accept()));
        layout.addWidget(&user, 0, 1);
        layout.addWidget(&pass, 1, 1);
        QPushButton button;
        button.setText("OK");
        connect(&button, SIGNAL(clicked()), &dialog, SLOT(accept()));
        layout.addWidget(&button, 2, 0, 1, 2, Qt::AlignCenter);
        dialog.setLayout(&layout);
        dialog.exec();
        authenticator->setUser(user.text());
        authenticator->setPassword(pass.text());
    }
}
