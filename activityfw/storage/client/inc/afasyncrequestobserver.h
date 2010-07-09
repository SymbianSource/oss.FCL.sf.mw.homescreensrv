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

#ifndef AFASYNCREQUESTOBSERVER_H
#define AFASYNCREQUESTOBSERVER_H
#include <QString>
#include <QPixmap>

class MAfAsyncRequestObserver
{
public:
    /**
     * Function inform observer about asynchronous request results
     * @param result - request result
     * @param requestType - request type
     * @param data - respons data
     */
    virtual void asyncRequestCompleated(int result,
                                        int requestType,
                                        const QString &data)=0;

    /**
     * Function inform observer about asynchronous request results
     * @param result - request result
     * @param requestType - request type
     * @param pixmap - pixmap respons
     */
    virtual void asyncRequestCompleated(int result,
                                        int requestType,
                                        const QPixmap& pixmap, 
                                        void* userData)=0;
    
    /**
     * Function inform observer about asynchronous request results
     * @param result - request result
     * @param requestType - request type
     */
    virtual void asyncRequestCompleated(int result,
                                        int requestType)=0;
};
#endif // AFASYNCREQUESTOBSERVER_H