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
* Description:  Shortcut plug-in extension data model
*
*/


#ifndef CAISCUTEXTDATAMODEL_H
#define CAISCUTEXTDATAMODEL_H

#include <e32base.h>
#include <badesca.h>

class CGulIcon;
class CAiScutExtData;

/**
 *  This class owns an array of CAiScutExtData items and provides an API to
 *  update popup line array and icon data for different targets.
 *  @since S60 v3.2
 */
class CAiScutExtDataModel : public CBase
    {
    private:
        friend class T_CAiScutExtDataModel;

    public:
        /**
         * Factory function
         * @since S60 v3.2
         */
        static CAiScutExtDataModel* NewL();


        /**
         * Factory function
         * @since S60 v3.2
         */
        static CAiScutExtDataModel* NewLC();

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CAiScutExtDataModel();
        
    private:
        /**
         * Constructor
         * @see CAiScutEngine
         * @since S60 v3.2
         */
        CAiScutExtDataModel();

        void ConstructL();

    public:  // New functions    
        /**
         * Sets the new popup line array for target. The old one is deleted. If
         * matching target definition is not found new CAiScutExtData object is
         * created.
         * @param aDefinition Target definition string
         * @param aLineArray Pointer to line array. Ownership is transferred
         * @return Pointer to new or existing CAiScutExtData object
         * @since S60 v3.2
         */
        CAiScutExtData* SetPopupLineArrayL( const TDesC& aDefinition, CDesCArray* aLineArray );

        /**
         * Resets popup line array. If also the icon is not set the CAiScutExtData
         * item is deleted and removed from the array.
         * @param aDefinition Target definition string
         * @since S60 v3.2
         */
        CAiScutExtData* ResetPopupLineArray( const TDesC& aDefinition );

        /**
         * Sets the new icon for target. The old one is deleted. If
         * matching target definition is not found new CAiScutExtData object is
         * created.
         * @param aDefinition Target definition string
         * @param aIcon Pointer to icon. Ownership is transferred
         * @return Pointer to new or existing CAiScutExtData object
         * @since S60 v3.2
         */
        CAiScutExtData* SetIconL( const TDesC& aDefinition, CGulIcon* aIcon );

        /**
         * Resets icon. If also the popup line array is not set the CAiScutExtData
         * item is deleted and removed from the array.
         * @param aDefinition Target definition string
         * @since S60 v3.2
         */
        CAiScutExtData* ResetIcon( const TDesC& aDefinition );

        /**
         * @return Array of CAiScutExtData objects
         * @since S60 v3.2
         */
        TArray<CAiScutExtData*> AiScutExtDataArray() const;

        /**
         * Deletes CAiScutExtData object and removes it from the array.
         * @param aDefinition Target definition string
         * @since S60 v3.2
         */
        void RemoveAiScutExtData( const TDesC& aDefinition );
        
    private: // New functions
        TInt FindAiScutExtData( const TDesC& aDefinition ) const;
        CAiScutExtData* CreateAiScutExtDataL( const TDesC& aDefinition );
        
    private: // data
        /**
         * Shortcut extension data items
         * Own.
         */
         RPointerArray<CAiScutExtData> iAiScutExtDataArray;
    };

#endif // CAISCUTEXTDATAMODEL_H

// End of File.
