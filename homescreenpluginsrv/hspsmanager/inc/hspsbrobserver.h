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
* Description:  Defination of class CHSPSBRObserver
*
*/


#ifndef C_CHSPSBROBSERVER_H
#define C_CHSPSBROBSERVER_H

#include <e32base.h>
#include <e32property.h>

class ChspsThemeServer;

/**
* @ingroup group_hspsserver
* Provides implementation to HSPS backup event listener
*
**/
NONSHARABLE_CLASS( CHSPSBRObserver ) : public CActive
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aServer Reference to listening server
        * @return pointer to CHSPSBRObserver object
        */
        static CHSPSBRObserver* NewL( ChspsThemeServer& aServer );
        
        /**
        * Destructor.
        */
        virtual ~CHSPSBRObserver();


    public: // New functions
        /**
        * Starts listening of backup events
        */
        void Start();

    public: // Functions from base classes
        
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        * @param aError error code
        */
        TInt RunError( TInt aError );


    private:

        /**
        * C++ default constructor.
        * @param aServer Reference to listening server
        */
        CHSPSBRObserver( ChspsThemeServer& aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        // Listening server reference
        ChspsThemeServer& iServer;

        // To receive backup events
        RProperty iProperty;

    };

#endif // C_CHSPSBROBSERVER_H
