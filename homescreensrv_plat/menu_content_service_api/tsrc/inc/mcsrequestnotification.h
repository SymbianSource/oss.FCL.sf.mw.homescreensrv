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
* Description:  
*
*/


#ifndef __MCSREQUESTNOTIFICATION_H_
#define __MCSREQUESTNOTIFICATION_H_

#include <e32base.h>
#include "mcsmenunotifier.h"

//class MMCSCallback;

/**
 * This class implements MCS SAPI core logic for Request Notification async requests.
 * 
 */
class CMCSReqNotification: public CActive
    {
    public:
    
        /**
        * Two-phased constructor.
        * @param aMCS MenuContentService object.
        * @param aCallback Callback Object for Change notification.
        * @return CMCSRunningApps reference.
        */
        static CMCSReqNotification* NewL(RMenu& aMCS);
        
        /**
         * Destructor.
        */
        virtual ~CMCSReqNotification();
        
        TInt GetStatus();
        
    private:

         /** 
         * Default constructor.
         */
         CMCSReqNotification();

         /** 
         * ConstructL
         * @param aMCS MenuContentService object.
         */
         void ConstructL(RMenu& aMCS);
         
         /**
         * Inherited from CActive class 
         */ 
         virtual void DoCancel();

         /**
         * Inherited from CActive class 
         */ 
         virtual void RunL();
      
    public :
        
        /** 
         * Starts the notification of events.
         * @param aFolderId folder id to observe the events.
         * @param aEvent events to notificy.
         */
         void StartNotificationL(TInt aFolderId, TInt aEvent );

  
    private:
        
        RMenuNotifier  iNotifier;
        /** Folder it to watch owned. */

        /** iMCS reference. Not owned. */
        RMenu iMCS;
        
        TInt iStat;
    };

#endif /*__MCSREQUESTNOTIFICATION_H_*/
