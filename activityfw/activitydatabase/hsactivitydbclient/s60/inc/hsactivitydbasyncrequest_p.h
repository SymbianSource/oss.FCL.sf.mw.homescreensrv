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
        EWaitGetThumbnail = GetThumbnail,
        ENotifyDataChange = NotifyChange
    };
    
private:
    static HsActivityDbAsyncRequestPrivate*
    NewLC(HsActivityDbAsyncRequestObserver &, 
          HsActivityDbClientPrivate &,
          TAsyncRequest,
          void* userData = 0);
    
public:
    
    static void waitActivityLD(HsActivityDbAsyncRequestObserver &, 
                               HsActivityDbClientPrivate &,
                               const QVariantHash &);
    
    static void notifyDataChangeLD(HsActivityDbAsyncRequestObserver &, 
                                   HsActivityDbClientPrivate &);
    
    static void getThumbnailLD(HsActivityDbAsyncRequestObserver &,
                               HsActivityDbClientPrivate &, 
                               QSize, 
                               const QString&, 
                               const QString&, 
                               void *);
    

    /**
     * Destructor
     */
    ~HsActivityDbAsyncRequestPrivate();

private:
    /**
     * Function create subscription to current ativity changes
     * @param condition - activity filetering rules
     */
    void waitActivity(const QVariantHash &condition);
    
    /**
     */
    void getThumbnail(QSize size, QString imagePath, QString  mimeType);
    
    void notifyDataChange();
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
