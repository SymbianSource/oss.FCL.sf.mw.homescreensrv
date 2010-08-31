/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: camificonengine.cpp
 *
 */

#include <QPainter>
#include <QPixmap>
#include <QFileInfo>
#include <QDebug>

#include <AknIconUtils.h> // avkon
#include <apgicnfl.h> // fbsbitmap

#include "camificonengine.h"

const int cacheSize = 4;

inline QString CaMifIconEngine::pmcKey(const QSize &size)
{ 
    return QString::number((size.width()<<11)|size.height(), 16);
}

CaMifIconEngine::CaMifIconEngine()
    : QIconEngineV2(),
      mMifFilename(),
      mCache(cacheSize),
      mBitmapCached(0),
      mMaskBitmapCached(0)
{
}

CaMifIconEngine::CaMifIconEngine(const CaMifIconEngine &other) :
    QIconEngineV2(other),
    mMifFilename(other.mMifFilename),
    mCache(cacheSize),
    mBitmapCached(0),
    mMaskBitmapCached(0)
{
}

CaMifIconEngine::~CaMifIconEngine()
{
    mCache.clear();
    delete mBitmapCached;
    delete mMaskBitmapCached;
}

void CaMifIconEngine::cacheBitmapL()
{
    CFbsBitmap *bitmap(0);
    CFbsBitmap *maskBitmap(0);

    TInt bitmapIndex = 0;
    TInt maskIndex = 1;
    TPtrC16 filename(
        reinterpret_cast<const TUint16*>(mMifFilename.utf16()));
    // it will change bitmap ids if it is mif (checking inside)
    AknIconUtils::ValidateLogicalAppIconId( filename,
        bitmapIndex, maskIndex );
    AknIconUtils::CreateIconLC( bitmap, maskBitmap, filename,
        bitmapIndex, maskIndex );
    mBitmapCached = bitmap;
    mMaskBitmapCached = maskBitmap;
    // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
    CleanupStack::Pop(2);
}
void CaMifIconEngine::getPixmapFromBitmap(const QSize &size, QPixmap& pixmap)
{
    if (!mBitmapCached || !mMaskBitmapCached) {
        TRAP_IGNORE(cacheBitmapL());
    }
    if (mBitmapCached && mMaskBitmapCached) {
        CFbsBitmap *bitmap = mBitmapCached;
        CFbsBitmap *maskBitmap = mMaskBitmapCached;
        AknIconUtils::SetSize(bitmap, TSize(size.width(), size.height()),
                        EAspectRatioPreservedAndUnusedSpaceRemoved);
    
        pixmap = pixmap.fromSymbianCFbsBitmap(bitmap);
        QPixmap mask;
        mask = mask.fromSymbianCFbsBitmap(maskBitmap);
        pixmap.setAlphaChannel(mask);
    }
}

QSize CaMifIconEngine::actualSize(const QSize &size,
    QIcon::Mode mode,
    QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);
    QString key(pmcKey(size));
    qDebug() << "pmckey: " << key;
    QSize realSize;
            
    if (mCache.contains(key)) {
        realSize = size;
    } else {
        QPixmap pixmap;
        TPtrC16 filename(
            reinterpret_cast<const TUint16*>(mMifFilename.utf16()));
        getPixmapFromBitmap(size, pixmap);
        if (!pixmap.isNull()) {
            QPixmap *cachedPixmap = new QPixmap(pixmap);
            if (mCache.insert(key, cachedPixmap)) {
                realSize = size;
                // mCache has taken ownership of cachedPixmap. 
                qDebug() << "pmckey INSERT";
            }
        }
    }
    return realSize;
}

QPixmap CaMifIconEngine::pixmap(const QSize &size,
    QIcon::Mode mode,
    QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);
    QPixmap pixmap;
    actualSize(size, mode, state);
    QString key = pmcKey(size);
    if (mCache.contains(key)) {
        pixmap = *mCache[key];
    }
    return pixmap;
}

void CaMifIconEngine::addPixmap(const QPixmap &pixmap,
    QIcon::Mode mode,
    QIcon::State state)
{
    Q_UNUSED(pixmap);
    Q_UNUSED(mode);
    Q_UNUSED(state);
}

void CaMifIconEngine::addFile(const QString &fileName,
    const QSize &size,
    QIcon::Mode mode,
    QIcon::State state)
{
    Q_UNUSED(size);
    Q_UNUSED(mode);
    Q_UNUSED(state);
    // Mif file name is recorded only the first time "addFile" function
    // is invoked (from QIcon constructor).
    if (mMifFilename.isEmpty() && !fileName.isEmpty()) {
        
        QString abs = fileName;
        if (fileName.at(0) != QLatin1Char(':')) {
            abs = QFileInfo(fileName).absoluteFilePath();
        }
        if (abs.endsWith(QLatin1String(".mif"), Qt::CaseInsensitive))
        {
            mMifFilename = abs;
            mMifFilename.replace('/', '\\');
            TRAP_IGNORE(cacheBitmapL());
        } 
    }
}

void CaMifIconEngine::paint(QPainter *painter, const QRect &rect,
                           QIcon::Mode mode, QIcon::State state)
{
    painter->drawPixmap(rect, pixmap(rect.size(), mode, state));
}

QString CaMifIconEngine::key() const
{
    return QLatin1String("mif");
}

QIconEngineV2 *CaMifIconEngine::clone() const
{
    return new CaMifIconEngine(*this);
}



