/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Task list entry
*
*/
#include <fbs.h>
#include "tsthumbnailprovider.h"
//------------------------------------------------------------------------------
TsThumbnailProvider::TsThumbnailProvider(MTsThumbnailObserver& observer, QObject* obj)
:
    mObserver(observer)
{
    connect(this, 
            SIGNAL(createThumbnail(QPixmap,int,const void*)),
            obj,
            SLOT(createThumbnail(QPixmap,int,const void*)));
    
    connect(obj, 
            SIGNAL(thumbnailCreated(QPixmap, const void*)),
            this,
            SLOT(thumbnailCreated(QPixmap, const void*)));
}
//------------------------------------------------------------------------------
void TsThumbnailProvider::createThumbnail(CFbsBitmap& source, int angle)
{
    emit createThumbnail(QPixmap::fromSymbianCFbsBitmap(&source), 
                         angle, 
                         this);
}

//------------------------------------------------------------------------------
void TsThumbnailProvider::thumbnailCreated(const QPixmap& thumbnail,
                                           const void *userData)
{
    if (this != userData) {
        return;
    }
    CFbsBitmap *bitmap(thumbnail.toSymbianCFbsBitmap());
    if(0 != bitmap) {
        mObserver.thumbnailCreated(*bitmap);
    }
    delete bitmap;
}
