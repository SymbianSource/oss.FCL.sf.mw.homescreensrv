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
* Description:
*
*/
#ifndef HSACTIVITYDBASYNCREQUESTPRIVATE_H
#define HSACTIVITYDBASYNCREQUESTPRIVATE_H
#include <e32base.h>
#include <QVariant>
#include <QPixmap>
#include <hsactivitydbasyncrequestobserver.h>
#include <activitycmd.h>

class HsActivityDbClientPrivate;

/**
 * Class is responsible for async. request handling
 */
class HsActivityDbAsyncRequestPrivate : public CActive

{
public:
    
    enum TAsyncRequest{
        EWaitActivity = WaitActivity,
        EWaitGetThumbnail = GetThumbnail
    };
    
private:
    /**
     * First step constructor
     */
    static HsActivityDbAsyncRequestPrivate*
    NewL(HsActivityDbAsyncRequestObserver &, 
         HsActivityDbClientPrivate &,
         TAsyncRequest,
         void* userData = 0);
    
public:
    
    static HsActivityDbAsyncRequestPrivate*
    newWaitActivityL(HsActivityDbAsyncRequestObserver &, 
            HsActivityDbClientPrivate &);
    
    static void
    getThumbnailLD(HsActivityDbAsyncRequestObserver &observer,
         HsActivityDbClientPrivate &session, 
         QSize size, 
         QString imagePath, 
         QString  mimeType, 
         void *userDdata);
    

    /**
     * Destructor
     */
    ~HsActivityDbAsyncRequestPrivate();

    /**
     * Function create subscription to current ativity changes
     * @param condition - activity filetering rules
     */
    void waitActivity(const QVariantHash &condition);
private:
    /**
     */
    void getThumbnail(QSize size, QString imagePath, QString  mimeType);

    /**
     */
    QPixmap copyPixmap(CFbsBitmap* bitmap);
    
protected:
    /**
     * Interface implementation.
     * @see void CActive::DoCancel()
     */
    void DoCancel();

    /**
     * Interface implementation.
     * @see void CActive::DoCancel()
     */
    void RunL();
private:
    /**
     * Constructor
     */
    HsActivityDbAsyncRequestPrivate(HsActivityDbAsyncRequestObserver &, 
                                    HsActivityDbClientPrivate &,
                                    TAsyncRequest,
                                    void* userData);
private:
    HsActivityDbAsyncRequestObserver &mObserver;
    HsActivityDbClientPrivate &mSession;
    const TAsyncRequest mRequestType;
    TPckgBuf<int> mTaskId;
    TPckgBuf<int> mDataSize;
    TPckgBuf<int> mBitmapId;
    HBufC* mBitmapPath;
    HBufC8* mBitmapMimeType;
    RBuf8 mDataBuf;
    void *const mUserData;
};
#endif // HSACTIVITYDBCLIENTPRIVATE_H
