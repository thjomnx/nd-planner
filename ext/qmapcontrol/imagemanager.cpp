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

#include "imagemanager.h"
namespace qmapcontrol
{
    ImageManager* ImageManager::m_Instance = 0;
    ImageManager::ImageManager(QObject* parent)
            :QObject(parent), emptyPixmap(QPixmap(1,1)), net(new MapNetwork(this)), doPersistentCaching(false)
    {
        emptyPixmap.fill(Qt::transparent);

        if (QPixmapCache::cacheLimit() <= 20000)
        {
            QPixmapCache::setCacheLimit(20000);	// in kB
        }
    }


    ImageManager::~ImageManager()
    {
        if (ImageManager::m_Instance != 0)
        {
            delete ImageManager::m_Instance;
        }
        delete net;
    }

    QPixmap ImageManager::getImage(const QString& host, const QString& url)
    {
        QPixmap pm;

        // Check if image is cached in memory or currently loading
        if (!QPixmapCache::find(url, pm) && !net->imageIsLoading(url))
        {
            // Check if image is cached on disk
            if (doPersistentCaching && tileExist(url))
            {
                loadTile(url,pm);
                QPixmapCache::insert(url, pm);
            }
            else
            {
                // Load from net
                net->loadImage(host, url);

                return emptyPixmap;
            }
        }

        return pm;
    }

    QPixmap ImageManager::prefetchImage(const QString& host, const QString& url)
    {
        // TODO See if this actually helps on the N900 & Symbian Phones
#if defined Q_WS_QWS || defined Q_WS_MAEMO_5 || defined Q_WS_S60
        // on mobile devices we don´t want the display refreshing when tiles are received which are
        // prefetched... This is a performance issue, because mobile devices are very slow in
        // repainting the screen
        prefetch.append(url);
#endif
        return getImage(host, url);
    }

    void ImageManager::receivedImage(const QPixmap pixmap, const QString& url)
    {
        qDebug() << "Image received for " << url;

        QPixmapCache::insert(url, pixmap);

        if (doPersistentCaching && !tileExist(url))
        {
            saveTile(url, pixmap);
        }

        if (!prefetch.contains(url))
        {
            emit(imageReceived());
        }
        else
        {
#if defined Q_WS_QWS || defined Q_WS_MAEMO_5 || defined Q_WS_S60
            prefetch.remove(prefetch.indexOf(url));
#endif
        }
    }

    void ImageManager::loadingQueueEmpty()
    {
        emit(loadingFinished());
        //((Layer*)this->parent())->removeZoomImage();
        //qDebug() << "size of image-map: " << images.size();
        //qDebug() << "size: " << QPixmapCache::cacheLimit();
    }

    void ImageManager::abortLoading()
    {
        net->abortLoading();
    }
    void ImageManager::setProxy(QString host, int port)
    {
        net->setProxy(host, port);
    }

    void ImageManager::setCacheDir(const QDir& path)
    {
        doPersistentCaching = true;
        cacheDir = path;
        if (!cacheDir.exists())
        {
            cacheDir.mkpath(cacheDir.absolutePath());
        }
    }

    bool ImageManager::saveTile(QString tileName,QPixmap tileData)
    {
        tileName.replace("/","-");

        QFile file(cacheDir.absolutePath() + "/" + tileName);

        //qDebug() << "writing: " << file.fileName();
        if (!file.open(QIODevice::ReadWrite )){
            qDebug()<<"error reading file";
            return false;
        }
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        tileData.save(&buffer, "PNG");

        file.write(bytes);
        file.close();
        return true;
    }
    bool ImageManager::loadTile(QString tileName,QPixmap &tileData)
    {
        tileName.replace("/","-");
        QFile file(cacheDir.absolutePath() + "/" + tileName);
        if (!file.open(QIODevice::ReadOnly )) {
            return false;
        }
        tileData.loadFromData( file.readAll() );

        file.close();
        return true;
    }
    bool ImageManager::tileExist(QString tileName)
    {
        tileName.replace("/","-");
        QFile file(cacheDir.absolutePath() + "/" + tileName);
        if (file.exists())
            return true;
        else
            return false;
    }
}