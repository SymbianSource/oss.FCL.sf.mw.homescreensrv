/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       Central Repository notifier and callback
*
*/


#ifndef CMCSSATNOTIFIER_H
#define CMCSSATNOTIFIER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>

/*
*   Callback for notifier.
*/
class MMcsSATNotifierCallback
    {
    public:

        /**
         * SATChangeL is called when the subscribed key has been changed
         * 
         */
        virtual void SATChangeL() = 0;

    };


// FORWARD DECLARATIONS
/**
 * Central repository notifier
 */
class CMcsSatNotifier : public CActive
    {
    public: // Construction and destruction

        /**
         * Creates an instance of CMenuSatNotifier implementation.
         * @param aCallBack Reference to notifier interface
         * @param aCategory application uid
         * @param aKey key for central repository
         */
    IMPORT_C   static CMcsSatNotifier* NewL( MMcsSATNotifierCallback* aCallback,
                                       TUid aCategory, 
                                       TUint aKey );

        /**
         * Destructor.
         */
        virtual ~CMcsSatNotifier();


    private: // From base classes

        /**
        * From CActive.
        */
        void DoCancel();
        void RunL();
        TInt RunError( TInt aError );

    private:
        
        // Constructor
        CMcsSatNotifier( MMcsSATNotifierCallback* aCallback, 
                                    TUid aCategory, 
                                    TUint aKey );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private: // Data

        // User side interface to Publish & Subscribe.
        RProperty iProperty;

        // Interface for notifying changes in SAT
        MMcsSATNotifierCallback* iCallback;
        
        // category uid
        TUid    iCategory;
        
        // key identifier
        TUint   iKey;

    };

#endif // CMCSSATNOTIFIER_H

// End of File
