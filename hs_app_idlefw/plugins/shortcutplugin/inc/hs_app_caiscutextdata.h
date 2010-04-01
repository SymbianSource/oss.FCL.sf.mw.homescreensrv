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
* Description: 
*
*/


#ifndef CAISCUTEXTDATA_H
#define CAISCUTEXTDATA_H

#include <e32base.h>
#include <badesca.h>

#include "hs_app_maiscutextdata.h"

class CGulIcon;

/**
 *  @since S60 v3.2
 */
class CAiScutExtData : public CBase, 
    public MAiScutExtData
    {
    public:
        /**
         * Factory function
         * @since S60 v3.2
         */
        static CAiScutExtData* NewL( const TDesC& aTargetDefinition );


        /**
         * Factory function
         * @since S60 v3.2
         */
        static CAiScutExtData* NewLC( const TDesC& aTargetDefinition );

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CAiScutExtData();
        
    private:
        /**
         * Constructor
         * @see CAiScutEngine
         * @since S60 v3.2
         */
        CAiScutExtData();

        void ConstructL( const TDesC& aTargetDefinition );

    public:  // New functions    
        /**
         * @since S60 v3.2
         */
        const TDesC& TargetDefinition() const;
     
         /**
         * Deletes old and stores new popup line array
         * @param aPopupLineArray Popup line array. Ownership is transferred.
         * @since S60 v3.2
         */
        void SwitchPopupLineArray( CDesCArray* aPopupLineArray );

        /**
         * @since S60 v3.2
         */
        void ResetPopupLineArray();

        /**
         * Deletes old and stores new icon
         * @param aIcon Icon. Ownership is transferred.
         * @since S60 v3.2
         */
        void SwitchIcon( CGulIcon* aIcon );

        /**
         * @since S60 v3.2
         */
        void ResetIcon();
       
    public:  //From MAiScutExtData
        const MDesCArray* PopupLineArray() const;        
        const CGulIcon* Icon() const;
        
    private: // data

        /**
         * Definition string of the target shortcut
         * Own.
         */
         HBufC* iTargetDefinition;

        /**
         * Popup line array
         * Own.
         */
         CDesCArray* iPopupLineArray;

        /**
         * Icon
         * Own.
         */
         CGulIcon* iIcon;
    };

#endif // CAISCUTEXTDATA_H

// End of File.
