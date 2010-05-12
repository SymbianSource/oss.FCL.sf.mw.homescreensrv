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

#ifndef HSCONTENTINFO_H
#define HSCONTENTINFO_H

// System includes
#include <e32base.h>
#include <s32strm.h>
#include <s32mem.h>

// Forward declarations

/**
 *  Content data
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CHsContentInfo ) : public CBase
    {
    public:
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CHsContentInfo* NewL();
    
    /**
     * Two-phased constructor.
     */    
    IMPORT_C static CHsContentInfo* NewL( RReadStream& aStream );
    
    /**
     * Two-phased constructor.
     */    
    IMPORT_C static CHsContentInfo* NewLC();
    
    /**
     * Clone.
     */
	IMPORT_C CHsContentInfo* CloneL();

    /**
     * Destructor.
     */
    ~CHsContentInfo();

private:
    /**
     * Constructor
     */
    CHsContentInfo();

    /** Second phase constructor */
    void ConstructL();

public:
    /**
     * Set Widget name. 
     *
     * @since S60 5.0
     * @param aName. Name of the widget.
     * @return Reference to this
     */
    IMPORT_C void SetNameL( const TDesC8& aName );
    
    /**
     * Set Widget name. 
     *
     * @since S60 5.0
     * @param aName. Name of the widget.
     * @return Reference to this
     */
    IMPORT_C void SetNameL( const TDesC& aName );

    /**
     * Set Widget's publisher Id . 
     *
     * @since S60 5.0
     * @param aPublisherId. Name of the publisher.
     * @return Reference to this
     */
    IMPORT_C void SetPublisherIdL( const TDesC& aPublisherId );
    
    /**
     * Set Widget id. 
     *
     * @since S60 5.0
     * @param aUid Static uid of the widget.
     * @return Reference to this
     */
    IMPORT_C void SetUidL( const TDesC8& aUid );
    
    /**
     * Set Widget plugin id. 
     *
     * @since S60 5.0
     * @param aId Dynamic id generated by hsps.
     * @return Reference to this
     */
    IMPORT_C void SetPluginIdL( const TDesC8& aId );

    /**
     * Set widget type. Possible values widget and template 
     *
     * @since S60 5.0
     * @param aType Widget type.
     */
    IMPORT_C void SetTypeL( const TDesC8& aType );

    /**
     * Access to widget name. 
     *
     * @since S60 5.0
     * @return Reference to name
     */
    IMPORT_C const TDesC& Name() const;

    /**
     * Access to widget's publisher id. 
     *
     * @since S60 5.0
     * @return Reference to publisher id
     */
    IMPORT_C const TDesC& PublisherId() const;

    /**
     * Access to widget uid. 
     *
     * @since S60 5.0
     * @return Uid or KNullDesC8
     */
    IMPORT_C const TDesC8& Uid() const;
    
    /**
     * Access to widget name. 
     *
     * @since S60 5.0
     * @return Reference to name
     */
    IMPORT_C HBufC8* NameAs8BitLC() const;

    /**
     * Access to widget plugin id. 
     *
     * @since S60 5.0
     * @return Plugin id or null if not set
     */
    IMPORT_C const TDesC8& PluginId() const;

    /**
     * Set widget type. Possible values widget and template 
     *
     * @since S60 5.0
     * @return Type or KNullDesC8.
     */
    IMPORT_C const TDesC8& Type() const;

    /**
     * Set Maximum number of widgets
     *
     * @param aMaxWidget maximum number of widgets
     */
    IMPORT_C void SetMaxWidgets( TInt aMaxWidget );
    
    /**
     * Get Maximum number of widgets
     *
     * @return number of widgets or defaule value 1
     */
    IMPORT_C TInt MaxWidgets() const;   
    
    /**
     * Externalize.
     *
     * @param aStream A stream to externalize to
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream );
    
    /**
     * Sets whether this widget can be added to the active HS page
     *
     * @since S60 5.0
     * @param aValue ETrue if can be added, EFalse otherwise
     */
    IMPORT_C void SetCanBeAdded( TBool aValue );

    /**
     * Queries whether this widget can be added to the active HS page
     *
     * @since S60 5.0
     * @return ETrue if can be added, EFalse otherwise
     */    
    IMPORT_C TBool CanBeAdded() const;

    /**
     * Sets whether this widget can be removed from the active HS page
     *
     * @since S60 5.0
     * @param aValue ETrue if can be added, EFalse otherwise
     */
    IMPORT_C void SetCanBeRemoved( TBool aValue );

    /**
     * Queries whether this widget can be removed from the active HS page
     *
     * @since S60 5.0
     * @return ETrue if can be removed, EFalse otherwise
     */    
    IMPORT_C TBool CanBeRemoved() const;

    /**
     * Sets this widget's installation time
     *
     * @since S60 5.0
     * @param aTime Installation time
     */        
    IMPORT_C void SetInstallationTime( TTime& aTime );

    /**
     * Gets this widget's installation time
     *
     * @since S60 5.0
     * @return Installation time
     */            
    IMPORT_C TTime InstallationTime() const;

    /**
     * Sets this widget's description
     *
     * @since S60 5.0
     * @param aDescription Description
     */                
    IMPORT_C void SetDescriptionL( const TDesC& aDescription );

    /**
     * Sets this widget's description
     *
     * @since S60 5.0
     * @param aDescription Description
     */                
    IMPORT_C void SetDescriptionL( const TDesC8& aDescription );
    
    /**
     * Gets this widget's description
     *
     * @since S60 5.0
     * @return Description
     */                    
    IMPORT_C const TDesC& Description() const;
    
    /**
     * Sets this widget's icon path
     *
     * @since S60 5.0
     * @param aPath Path
     */                    
    IMPORT_C void SetIconPathL( const TDesC& aIconPath );

    /**
     * Gets this widget's icon path
     *
     * @since S60 5.0
     * @return Path
     */                    
    IMPORT_C const TDesC& IconPath() const;
    
    /**
     * Internalize
     * @param aStream A stream where the class is internalized
     *
     * @since S60 5.0
     */                    
    IMPORT_C void InternalizeL( RReadStream& aStream );    

    /**
     * Returns size of a descriptor needed to externalize the class
     *
     * @since S60 5.0
     */
    IMPORT_C TInt Size();
    
    /**
     * Set WRT widget publisher uid. 
     *
     * @since S60 5.0
     * @param aUid Publisher uid of the wrt widget.
     * @return Reference to this
     */
    IMPORT_C void SetPublisherUidL( const TDesC8& aUid );
    
    /**
     * Access to WRT widget publisher uid.
     * 
     * @since S60 5.0
     * @return PublisherUid or KNullDesC8
     */
    IMPORT_C const TDesC8& PublisherUid() const;

    /**
     * Sets whether this is a WRT widget
     * 
     * @since S60 5.0
     * @param aIsWrt, ETrue if is WRT
     */    
    IMPORT_C void SetIsWrt( TBool aIsWrt );
    
    /**
     * Returns whether this is a WRT widget
     * 
     * @since S60 5.0
     * @return ETrue if WRT, EFalse otherwise
     */    
    IMPORT_C TBool IsWrt() const;

    /**
     * Sets whether an application confguration or a view is full
     * 
     * @since S60 5.0
     * @param aIsFull, ETrue if application configuration or a view is full
     */    
    IMPORT_C void SetIsFull( TBool aIsFull );
    
    /**
     * Returns whether an application configuration or a view is full
     * 
     * @since S60 5.0
     * @return ETrue if application configuration or view is full, 
     *         EFalse otherwise
     */    
    IMPORT_C TBool IsFull() const;

    /**
     * Marshals Content Info data to a descriptor
     * 
     * @since S60 5.0
     * @return Descriptor containing the externalized Content Info data
     */
    IMPORT_C HBufC8* MarshalL();

private: // Data
    /**
     * Widget name, Owned
     */
    HBufC* iName;
    
    /**
     * Widget uid, Owned
     */
    HBufC8* iUid;
    
    /**
     * Widget plugin id, Owned
     */
    HBufC8* iPluginId;
    
    /**
     * Widget type, Owned
     */
    HBufC8* iType;
    
    /**
     * Publisher Id, Owned
     */
    HBufC* iPublisherId;

    /**
     * Description, Owned
     */
    HBufC* iDescription;

    /**
     * Icon path, Owned
     */
    HBufC* iIconPath;
    
    /**
     * Max number of instances allowed 
     */
    TInt iMaxInstance;

    /**
     * Flag to determine whether this widget 
     * can be added to the active HS page.
     */
    TBool iCanBeAdded;

    /**
     * Flag to determine whether this widget 
     * can be removed from the active HS page.
     */    
    TBool iCanBeRemoved;

    /**
     * Installation time    
     */        
    TTime iInstallationTime;
    
    /**
     * WRT Widget publisher uid, Owned
     */
    HBufC8* iPublisherUid;
    
    /**
     * Flag to indicate whether this is a WRT widget
     */
    TBool iIsWrt;

    /**
     * Flag to indicate wheter an application configuration or a view is full
     */
    TBool iIsFull;
    };

#endif // HSCONTENTINFO_H

// End of file
