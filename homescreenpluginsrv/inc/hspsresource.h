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
* Description:  Declaration of a file resource
*
*/


#ifndef hspsRESOURCE_H
#define hspsRESOURCE_H

#include <e32base.h>
#include <apmstd.h> // MIME
#include <s32mem.h> // streams
 
const TInt KMaxTagsLength = 50;

/**
* ThspsLockingPolicy
* Locking policy flag-definitions for HSPS. These are bit-masked.
*/
enum ThspsLockingPolicy
  {
  EhspsUnlocked           = 0x0000, //0b0000000000000000,			Resource is replaceable
  EhspsLocked             = 0x0001  //0b0000000000000001,			The resource is always fetched from ROM and it cannot be replaced
  };
    
/**
 * Resource file extension definitions for HSPS.
 */
_LIT(KODTFileExtension, ".o0000");
_LIT(KDTDFileExtension, ".dtd");
_LIT(KXMLFileExtension, ".xml");
_LIT(KDATFileExtension, ".dat");
    

/**
* ThspsResourceType
* Resource-type definitions for HSPS.
*/
enum ThspsResourceType
    {
    /**
    * EResourceNone. No type at all - juts a file path.
    */
    EResourceNone,
            
    /**
    * EResourceODT. Resource is ODT (HSPS Application Theme's binary format)
    */
    EResourceODT,
    
    /**
    * EResourceDTD.
    */
    EResourceDTD,
    
    /**
    * EResourceXML.
    */
    EResourceXML,
    
    /**
    * EResourceDAT.
    */
    EResourceDAT,
    
    /**
    * EResourceRES. Resource is some media content.
    */
    EResourceRES,
            
    /**
    * EResourceOther. Other extension. 
    */
    EResourceOther
    
    };


/**
 * ChspsResource is HSPS-utility class defining resource-structure.
 *
 * @lib hspsResource.lib
 * @since S60 5.0
 * @ingroup group_hspsresource 
 */
class ChspsResource : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        IMPORT_C static ChspsResource* NewL();
        
        
        /**
        * Makes a clone of this class.
        * 
        * @since S60 5.0
        */
        IMPORT_C ChspsResource* CloneL();
        
        /**
        * Destructor.
        */
        virtual ~ChspsResource();

    public: // New functions
        
        /**
        * Sets value of iLockingPolicy data member.
        * 
        * @since S60 5.0
        * @param aLockingPolicy is new value for iLockingPolicy data member.
        */
        IMPORT_C void SetLockingPolicy( ThspsLockingPolicy aLockingPolicy );
        
        /**
        * Gets value of iLockingPolicy data member.
        * 
        * @since S60 5.0
        * @return ThspsLockingPolicy iLockingPolicy.
        */
        IMPORT_C ThspsLockingPolicy LockingPolicy() const;
        
        /**
        * Sets value of iResourceType data member.
        * 
        * @since S60 5.0
        * @param aResourceType is new value of iResourceType data member.
        */    
        IMPORT_C void SetResourceType( ThspsResourceType aResourceType );
        
        /**
        * Gets value of iResourceType data member.
        * 
        * @since S60 5.0
        * @return ThspsResourceType iResourceType.
        */
        IMPORT_C ThspsResourceType ResourceType() const;
        
        /**
        * Sets value of iResourceId data member.
        * 
        * @since S60 5.0
        * @param aResourceId is new value of iResourceId data member.
        */
        IMPORT_C void SetResourceIdL( const TDesC& aResourceId );
        
        /**
        * Gets value of iResourceId data member.
        * 
        * @since S60 5.0
        * @return TDesC& iResourceId.
        */
        IMPORT_C const TDesC& ResourceId() const;
        
        /**
        * Sets value of iFileName data member.
        * 
        * @since S60 5.0
        * @param aFileName is new iFileName value.
        */
        IMPORT_C void SetFileNameL( const TDesC& aFileName );
        
        /**
        * Gets value of iFileName data member.
        * 
        * @since S60 5.0
        * @return TDes& iFileName.
        */
        IMPORT_C const TDesC& FileName() const;
        
        /**
        * Sets value of iMimeType data member.
        *  
        * @since S60 5.0
        * @param aDataType is a new value of iMimeType.
        */
        IMPORT_C void SetMimeTypeL( const TDataType aDataType );
        
        /**
        * Get iMimeType of this ChspsResource-object.
        * 
        * @since S60 5.0
        * @return TDataType iMimeType.
        */
        IMPORT_C TDataType MimeType() const;
        
        /**
        * Externalizes ChspsResource-object's member data.
        * 
        * @since S60 5.0
        * @param aStream is a write stream given.
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream) const;
        
        /**
        * Internalizes ChspsResource-object's member data.
        * 
        * @since S60 5.0
        * @param aStream is data read stream given.
        */
        IMPORT_C void InternalizeL( RReadStream& aStream);
        
        /**
        * Set to ELangNone if the resource is common to all locales,
        * otherwise it's language specific.
        * 
        * @since S60 5.0
        * @param aLanguage is new value of iLanguage data member.
        */        
        IMPORT_C void SetLanguage( const TLanguage& aLanguage );
        
        /**
        * Gets value of iLanguage data member.
        * 
        * @since S60 5.0
        * @return TLanguage& iLanguage.
        */
        IMPORT_C const TLanguage& Language() const;

        /**
         * Sets value of the iConfigurationUid member
         * @since S60 5.0
         * @param aConfUid Configuration UID
         */
        IMPORT_C void SetConfigurationUid( const TInt aConfUid );
        
        /**
         * Gets value of the iConfigurationUid member
         * @since S60 5.0
         * @return UID
         */
        IMPORT_C TInt ConfigurationUid() const;
        
        /**
         * Sets value of the iTags member
         * @since S60 5.0
         * @param aTag Tags
         */
        IMPORT_C void SetTagsL( const TDesC& aTag );
        
        /**
         * Gets value of the iTags member
         * @since S60 5.0
         * @return tags descriptor
         */
        IMPORT_C const TDesC& Tags() const;
        
    private:

        /**
        * C++ default constructor.
        */
        ChspsResource();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:     // Data
        
        // Not used?
        ThspsLockingPolicy iLockingPolicy;
        
        // Resource type
        ThspsResourceType iResourceType;
        
        // Language of the resource
        TLanguage iLanguage;
        
        // Identifies the configuration that owns the resource
        TInt iConfigurationUid;
        
        // Filename without the extension
        HBufC* iResourceID;
        
        // Full path and name
        HBufC* iFileName;
                
        TDataType iMimeType;
        
        // Tags seperated with a colon
        HBufC* iTags;

         // Reserved pointer for future extension
        TAny* iReserved;
    };

#endif      // hspsRESOURCE_H   
// End of File
