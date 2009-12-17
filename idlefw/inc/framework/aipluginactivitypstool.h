/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plugin activity PS tool
*
*/


#ifndef AIPLUGINACTIVITYPSTOOL_H
#define AIPLUGINACTIVITYPSTOOL_H

#include <e32base.h> // TUid
#include <e32property.h> // RProperty

struct TAiPublisherInfo;

NONSHARABLE_CLASS(CAiPluginActivityRegistry) : public CBase
    {
    public:
    
        static CAiPluginActivityRegistry* NewL();
    
        TInt SetPluginActive( const TAiPublisherInfo& aPubInfo );
                               
        void CleanRegistry(); 
                              
        ~CAiPluginActivityRegistry();
        
    private:
    
    // construction
    
        void ConstructL();
    
        CAiPluginActivityRegistry();
        
    // new methods
    
        /**
         * Update PS registry count register.
         */
        TInt UpdateCountRegister();
        
        /**
         * Update PS registry ordinal register.
         */
        TInt UpdateOrdinalRegister( TInt aPluginUid );

        /**
         * Update plugins name register.
         */
        TInt UpdateNameRegister( TInt aPluginUid,
                                 const TDesC& aName );
        
        /**
         * Clean last added entry. In case of failure.
         */
        void CleanLastEntry( TInt aPluginUid,
                             TInt aOrdinal,
                             TInt aLastCount );
    
    private: // Members
    
        /**
         * Ordinal in registry. Ascending.
         */
        TInt iRegistryOrdinal;

        /**
         * Plugin count in registry.
         */
        TInt iPluginCount;
        
        /**
         * PS property that is updated.
         */
        RProperty iProperty;
        
    };

#endif // AIPLUGINACTIVITYPSTOOL_H

