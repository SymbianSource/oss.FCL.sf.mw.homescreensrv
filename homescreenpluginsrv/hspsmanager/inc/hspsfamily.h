/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef HSPSFAMILY_H_
#define HSPSFAMILY_H_

#include <e32base.h>
#include <w32std.h>

#include "hspsthememanagement.h"



/**
*  @ingroup group_hspsserver
*  ChspsFamily is used to get UI's resolution and orientation.
*
*  @lib hspsThemeServer.exe 
*  @since S60 9.2
*/
class ChspsFamily
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * Two-phased constructor.
        */
        static ChspsFamily* NewL();
        
        /**
        * ~ChspsFamilyListener
        * Destructor.
        */
        virtual ~ChspsFamily();
        
        /**
         * Retrieves family type from a string
         * @since S60 9.2 
         */
        static ThspsFamily GetFamilyType( 
                const TDesC8& aFamilyString );
        
        /**
         * Retrieves family type from the current resolution.
         * @since S60 5.0
         * @return Family id
         */
        ThspsFamily GetFamilyType();

        /**
         * Retrieves window server session
         * @since S60 9.2
         * @return Window server session
         */
        inline RWsSession& WsSession();
        
    protected:

        /**
        * ConstructL
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    protected:
        /**
        * C++ default constructor.
        */
        ChspsFamily();
        
        
        /**
         * Retrieves used display code.
         */
        void GetFamilyString(        
                TDes8& aFamily );                
        
    private:    // Data
        
        // Window server session.
        RWsSession iWsSession;   
        
        // Client-side handle to a server-side window group.
        RWindowGroup iWindowGroup;
        
        // Screen device, own.
        CWsScreenDevice* iScreenDevice;
        
    };


// Inline methods


// -----------------------------------------------------------------------------
// ChspsFamily::WsSession
// -----------------------------------------------------------------------------
//
RWsSession& ChspsFamily::WsSession()
	{
	return iWsSession;
	}



#endif /* HSPSFAMILY_H_ */

// End of File
