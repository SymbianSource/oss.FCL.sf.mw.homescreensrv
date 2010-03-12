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
* Description:  Class represents an ODT (Object Description Tree).
*
*/


#ifndef hspsODT_H
#define hspsODT_H

#include <e32base.h>

class RWriteStream;
class RReadStream;
class ChspsDomDocument;
class ChspsResource;

const TInt KArrayGranularity = 5;
const TInt KHeaderDataLength = KMaxFileName;
const TInt KMaxDescLength = 255;

/**
*  Class represents an Object Description Tree of HSPS.
*  ODT is cabable of marshaling itself into HBufC8 descriptor and unmarshaling
*  from descriptor stream.
*  A Header is an ODT instance which has not internalized it's resources 
*  nor it's DOM (XML payload).
*
*  @lib hspsODT.lib
*  @since S60 5.0
*  @ingroup group_hspsodt 
*/
class ChspsODT : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        IMPORT_C static ChspsODT* NewL();
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        * @param aStreamData The read stream.
        */
        IMPORT_C static ChspsODT* NewLC( const TDesC8& aStreamData );
        
        /**
        * Destructor.
        */
        virtual ~ChspsODT();
    
    public:        
        /**
        * Externalizes the object.
        * 
        * @since S60 5.0
        * @param aStream The write stream.
        */ 
        IMPORT_C void ExternalizeL( RWriteStream& aStream) const;
        
        /**
        * Internalizes the object.
        * 
        * @since S60 5.0
        * @param aStream The read stream.
        */ 
        IMPORT_C void InternalizeL( RReadStream& aStream);

    public: // New functions
        
        /**
        * Marshals the ODT header into descriptor stream.
        * 
        * @since S60 5.0
        * @return HBufC8* A descriptor object containing the ODT header.
        */ 
        IMPORT_C HBufC8* MarshalHeaderL() const;
        
        /**
        * Unmarshals the header information from the stream and returns
        * ODT instance.
        * 
        * @since S60 5.0
        * @param aStreamData Descriptor stream containing the ODT header data.
        * @return ChspsODT* An ODT object prefilled with the header information.
        */ 
        IMPORT_C static ChspsODT* UnMarshalHeaderLC( const TDesC8& aStreamData );
        
        /**
        * Unmarshals the header information from the stream and saves the data
        * into same instance.
        * 
        * @since S60 5.0
        * @param aStreamData Descriptor stream containing the ODT header data.
        */ 
        IMPORT_C void UnMarshalHeaderL( const TDesC8& aStreamData );
        
        /**
        * Externalizes the ODT header.
        * 
        * @since S60 5.0
        * @param aStream The read stream.
        */ 
        IMPORT_C void ExternalizeHeaderL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes the ODT header.
        * 
        * @since S60 5.0
        * @param aStream The read stream.
        */ 
        IMPORT_C void InternalizeHeaderL( RReadStream& aStream );
        
        /**
        * Adds a resource-object into ODT.
        * 
        * @since S60 5.0
        * @param aResource ChspsResource to add. ODT takes ownership.
        */ 
        IMPORT_C void AddResourceL( ChspsResource* aResource );
        
        /**
         * Checks duplicates.
         * 
         * @since S60 5.0
         * @param aResource ChspsResource to check.
         * @return 0 if there were no duplicate resources, index otherwise.
         */
        TInt CheckResourceL( ChspsResource* aResource );
        
        /**
        * Deletes a resource from the ODT.
        * 
        * @since S60 5.0
        * @param aIndex Index of the resource to be deleted.
        */ 
        IMPORT_C void DeleteResourceL( TInt aIndex );

        /**
        * Deletes all resources from the ODT.
        * 
        * @since S60 5.0
        */ 
        IMPORT_C void DeleteAllResources();        
        
        /**
        * Gets a resource.
        * 
        * @since S60 5.0
        * @param aIndex Index of the resource to be retrieved.
        * @return A ChspsResource-object to be accessed. Ownership remains.
        */ 
        IMPORT_C ChspsResource& ResourceL( TInt aIndex ) const;
        
        /**
        * Amount of resources in ODT.
        * 
        * @since S60 5.0
        * @return TInt Amount of elements.
        */ 
        IMPORT_C TInt ResourceCount() const;
        
        /**
        * Externalizes the Resource List.
        * 
        * @since S60 5.0
        * @param aStream The read stream.
        */ 
        IMPORT_C void ExternalizeResourceListL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes the Resource List.
        * 
        * @since S60 5.0
        * @param aStream The read stream.
        */ 
        IMPORT_C void InternalizeResourceListL( RReadStream& aStream );
       
        
        /**
        * Set Uid of the application or interface, which uses the theme.
        * 
        * @since S60 5.0
        * @param aUid The root UID.
        */ 
        IMPORT_C void SetRootUid( TInt aUid );
        
        /**
        * Get RootUid.
        * 
        * @since S60 5.0
        * @return TInt The root UID.
        */ 
        IMPORT_C TInt RootUid() const;
        
        /**
        * Set providerUid.
        * 
        * @since S60 5.0
        * @param aUid The UID of the theme provider.
        */ 
        IMPORT_C void SetProviderUid( TInt aUid );
        
        /**
        * Get ProviderUid
        * 
        * @since S60 5.0
        * @return TInt The UID of the theme provider.
        */ 
        IMPORT_C TInt ProviderUid() const;
        
        /**
        * Set ThemeUid.
        * 
        * @since S60 5.0
        * @param aUid Theme UID.
        */ 
        IMPORT_C void SetThemeUid( TInt aUid );
        
        /**
        * Get ThemeUid.
        * 
        * @since S60 5.0
        * @return TInt Theme UID.
        */ 
        IMPORT_C TInt ThemeUid() const;
        
        /**
        * Set ProviderName.
        * 
        * @since S60 5.0
        * @param aName The name of the theme provider.
        */ 
        IMPORT_C void SetProviderNameL( const TDesC& aName );
        
        /**
        * Get ProviderName.
        * 
        * @since S60 5.0
        * @return TDesC& The name of the theme provider.
        */ 
        IMPORT_C const TDesC& ProviderName() const;
        
        /**
        * Set ThemeFullName.
        * 
        * @since S60 5.0
        * @param aName Themes' name.
        */ 
        IMPORT_C void SetThemeFullNameL( const TDesC& aName );
        
        /**
        * Get ThemeFullName.
        * 
        * @since S60 5.0
        * @return TDesC& The name of the theme.
        */ 
        IMPORT_C const TDesC& ThemeFullName() const;
        
        /**
        * Set ThemeShortName.
        * 
        * @since S60 5.0
        * @param aName Themes' short name.
        */ 
        IMPORT_C void SetThemeShortNameL( const TDesC& aName );
        
        /**
        * Get ThemeShortName
        * 
        * @since S60 5.0
        * @return TDesC& The short name of the theme.
        */ 
        IMPORT_C const TDesC& ThemeShortName() const;
        
        /**
        * Set ThemeVersion.
        * 
        * @since S60 5.0
        * @param aVersion Themes' version.
        */ 
        IMPORT_C void SetThemeVersionL( const TDesC& aVersion );
        
        /**
        * Get ThemeVersion.
        * @since S60 5.0
        * @return TDesC& The version of the theme.
        */ 
        IMPORT_C const TDesC& ThemeVersion() const;
        
        /**
        * Set OdtLanguage.
        * 
        * @since S60 5.0
        * @param aLanguage Themes' language.
        */ 
        IMPORT_C void SetOdtLanguage( TInt aLanguage );
        
        /**
        * Get OdtLanguage.
        * 
        * @since S60 5.0
        * @return TInt OdtLanguage.
        */ 
        IMPORT_C TInt OdtLanguage() const;
        
        /**
        * Set status flag-definitions for themes.
        * 
        * @since S60 5.0
        * @param aFlags Flags to be set.
        */ 
        IMPORT_C void SetFlags( TUint aFlags );
        
        /**
        * Get status flag-definitions for themes.
        * 
        * @since S60 5.0
        * @return TUint status flag-definitions.
        */ 
        IMPORT_C TUint Flags() const;
        
        /**
        * Get the DomDocument.
        * 
        * @since S60 5.0
        * @return ChspsDomDocument& Dom document.
        */ 
        IMPORT_C ChspsDomDocument& DomDocument() const;
        

        /**
        * Clones this ChspsODT.
        * 
        * @since S60 5.0
        * @return ChspsODT* pointer to the cloned ODT.
        */ 
        IMPORT_C ChspsODT* CloneL() const;

        /**
        * Clones given ODT to this ODT.
        * 
        * @since S60 5.0
        * @param	aODT	Source ODT.
        */         
        IMPORT_C void CloneL( ChspsODT& aODT );        
        
        /**
        * Clones the aDom and sets it as this ChspsODT's DomDocument.
        * 
        * @since S60 5.0
        * @param aDom Dom to be cloned.  
        */ 
        IMPORT_C void CopyDomDocumentL( ChspsDomDocument& aDom );
        
        /**
        * Set configurationt type.
        * 
        * @since S60 5.0
        * @param aType Type. 
        */ 
        IMPORT_C void SetConfigurationType( TUint aType );
        
        /**
        * Get configuration type.
        * 
        * @since S60 5.0
        * @return TUint configuration type.
        */ 
        IMPORT_C TUint ConfigurationType() const;

        /**
        * Set package version.
        * 
        * @since S60 5.0
        * @param aVersion Version. 
        */ 
        IMPORT_C void SetPackageVersionL( const TDesC& aVersion );
        
        /**
        * Get configuration type.
        * 
        * @since S60 5.0
        * @return TDesC package version
        */ 
        IMPORT_C const TDesC& PackageVersion() const;
        
        /**
        * Set family.
        * Identifies the resolution and user interface for which 
        * the resource/widget was designed for (e.g. vga, qhd_tch).
        * 
        * @since S60 5.0
        * @param aFamily Family mask (supported resolution bits set)
        */ 
        IMPORT_C void SetFamily( const TUint32 aFamilyMask );
        
        /**
        * Get family.
        * 
        * @since S60 5.0
        * @return The family mask
        */ 
        IMPORT_C TUint32 Family() const;

        /**
        * Set Widget instance count
        * 
        * @since S60 5.0
        * @param aMultiInstance multiinstance.
        */ 
        IMPORT_C void SetMultiInstance( TInt aMultiInstance );

        /**
        * Get Widget instance count
        * 
        * @since S60 5.0
        * @return TInt multiinstance.
        */ 
        IMPORT_C TInt MultiInstance() const;
        
        /**
         * Sets description of the widget
         * @since S60 5.0
         * @param aDesc Text
         */
        IMPORT_C void SetDescriptionL( const TDesC& aDesc );

        /**
         * Gets description of the widget
         * @since S60 5.0
         * @return text
         */
        IMPORT_C const TDesC& Description() const;
        
        /**
         * Sets path of the logo file         
         * @since S60 5.0
         * @param aPath Path to the file
         */
        IMPORT_C void SetLogoFileL( const TDesC& aPath );
        
        /**
         * Gets path to the logo file         
         * @since S60 5.0
         * @return path
         */
        IMPORT_C const TDesC& LogoFile() const;
        
        /**
         * Sets path of the preview file         
         * @since S60 5.0
         * @param aPath Path to the file
         */
        IMPORT_C void SetPreviewFileL( const TDesC& aPath );
        
        /**
         * Gets path to the preview file         
         * @since S60 5.0
         * @return path
         */
        IMPORT_C const TDesC& PreviewFile() const;
        
    private:

        /**
        * C++ default constructor.
        */
        ChspsODT();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        /**
        * Helper to ODT cloning. Prevents duplicate code in two clone methods.
        * 
        * @param aSource    Source ODT
        * @param aTarget    Target ODT
        */
        static void CopyODTDataL( const ChspsODT& aSource, ChspsODT& aTarget );
        
    private:    // Data

        // Family mask (bits for e.g. vga, qhd_tch etc resolutions)
        TUint32 iFamilyMask;
        
    	// Configuration type
    	TUint	iConfigurationType;
    	
    	// Application or interface uid
        TUint  iRootUid;
        
        // The publisher
        TUint  iProviderUid;
        
        // Identifies specific configuration
        TUint  iThemeUid;
        HBufC* iProviderName;
        HBufC* iThemeFullName;
        HBufC* iThemeShortName;
        
        // Revision number
        HBufC* iThemeVersion;
        
        // Indicates whether we are able to internalize the instance        
        HBufC* iPackageVersion;
        
        // Language of the ODT instance
        TInt   iLanguage;
        
        // Multiple instances allowed/not
        TInt   iMultiInstance;
        
        // Description of the widget
        HBufC* iDescription;
        
        // Path to the logo file
        HBufC* iLogoFile;
        
        // Path to the preview file
        HBufC* iPreviewFile;
        
        // ThspsThemeStatus
        TUint  iFlags;
                        
        // Pointer to list of unlocalized resources
        CArrayPtrSeg<ChspsResource>* iResourceList;
                
        ChspsDomDocument* iDomDocument;
        
        // Reserved pointer for future extension
        TAny* iReserved;
    };

#endif      // hspsODT_H   
            
// End of File
