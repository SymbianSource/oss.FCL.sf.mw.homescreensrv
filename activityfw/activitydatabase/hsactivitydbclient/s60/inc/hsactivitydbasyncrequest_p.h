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
#include <hsactivitydbasyncrequestobserver.h>

class HsActivityDbClientPrivate;

/**
 * Class is responsible for async. request handling
 */
class HsActivityDbAsyncRequestPrivate : public CActive

{
public:
    /**
     * First step constructor
     */
    static HsActivityDbAsyncRequestPrivate*
    NewL(HsActivityDbAsyncRequestObserver &, HsActivityDbClientPrivate &);

    /**
     * First step constructor
     */
    static HsActivityDbAsyncRequestPrivate*
    NewLC(HsActivityDbAsyncRequestObserver &, HsActivityDbClientPrivate &);

    /**
     * Destructor
     */
    ~HsActivityDbAsyncRequestPrivate();

    /**
     * Function create subscription to current ativity changes
     * @param condition - activity filetering rules
     */
    void waitActivity(const QVariantHash &condition);
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
                                    HsActivityDbClientPrivate &);
private:
    HsActivityDbAsyncRequestObserver &mObserver;
    HsActivityDbClientPrivate &mSession;
    int mRequestType;
    RBuf8 mDataBuf;
    TPckgBuf<int> mDataSize;
};
#endif // HSACTIVITYDBCLIENTPRIVATE_H
