/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Request Notification parameters
*
*
*/


#ifndef hspsREQNOTIFPARAM_H
#define hspsREQNOTIFPARAM_H

#include <e32base.h>
 
const TInt KMaxReqNotifDataLength8 = 5*sizeof(TUint32)+sizeof(TBool)+KMaxFileName;

class RDesReadStream;
class RDesWriteStream;

/**
 *  ChspsRequestNotificationParams is HSPS class for asuynchronous client's notification handling.
 *
 *  @lib hspsResult.lib
 *  @since S60 5.0
 *  @ingroup group_hspsresult
 */
class ChspsRequestNotificationParams : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        IMPORT_C static ChspsRequestNotificationParams* NewL();
        
        /**
        * Destructor.
        */
        virtual ~ChspsRequestNotificationParams();

    public: // New functions
              
        /**
        * ExternalizeL() writes ChspsRequestNotificationParams object to a given stream.
        * 
        * @since S60 5.0
        * @param aStream a write stream given.
        */
        IMPORT_C void ExternalizeL( RDesWriteStream& aStream) const;
        
        /**
        * InternalizeL() reads ChspsRequestNotificationParams from given stream.
        * 
        * @since S60 5.0
        * @param aStream is astream given.
        */
        IMPORT_C void InternalizeL( RDesReadStream& aStream);
        
        /**
        * GetDataLenght() calculates data length of ChspsRequestNotificationParam object.
        * 
        * @since S60 5.0
        * @return Length of the data in ChspsRequestNotificationParam object.
        */
        IMPORT_C TInt GetDataLength() const;
        
        /**
        * Resets hspsRequestNotificationParam data members.
        * 
        * @since S60 5.0
        */
        IMPORT_C void ResetData();
        
        /**
         * Set Name.
         * 
         * @since S60 5.0
         * @param aName Themes' name.
         */ 
        IMPORT_C void SetNameL( const TDesC& aName );
                
        /**
         * Get Name.
         * 
         * @since S60 5.0
         * @return TDesC& The name of the theme.
         */ 
         IMPORT_C const TDesC& Name() const;
               
    private:

        /**
        * C++ default constructor.
        */
        ChspsRequestNotificationParams();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
        // Event that are notified 
        TInt iEvent;
        // AppUid of the application that notification concerns 
        TInt iAppUid;
        // AppUid of the application that notification concerns 
        TInt iAppConfUid;
        // AppUid of the originator requested the configuration change
        TUint iOrigUid;
        // PluginUid of the plugin that notification concerns.
        TInt iPluginUid;
        // Count of the plugins that notifications concerns;
        TInt iCount;
        // PluginIds of the plugins that notification concerns.
        RArray<TInt> iPluginIds;
        // Plugin name of the plugin that notification concerns
        HBufC* iName;
           
    private:     // Data

        // Reserved pointer for future extension
        TAny* iReserved;
    };

#endif      // hspsRESULT_H   
            
// End of File
