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


#ifndef M_AIPROPERTYEXTENSION_H
#define M_AIPROPERTYEXTENSION_H

#include <e32std.h>

/**
 * Maximum length for Content publisher name
 */
const TInt KAiPublisherNameMaxLength = 128;

/**
 * Maximum length for Content publisher namespace
 */
const TInt KAiPublisherNamespaceMaxLength = 32;

/**
 * Content publisher name buffer
 */
typedef TBuf<KAiPublisherNameMaxLength> TAiPublisherName;

/**
 * Content publisher namespace buffer
 */
typedef TBuf8<KAiPublisherNamespaceMaxLength> TAiPublisherNamespace;

/**
 * Contains information about Content publisher plug-in.
 */
struct TAiPublisherInfo
    {
    TAiPublisherInfo() 
        : iUid( TUid::Null() ), 
          iName( KNullDesC ), 
          iNamespace( KNullDesC8 )
        {
        }
    
    inline TAiPublisherInfo& operator= (const TAiPublisherInfo& aInfo)
        {
        iUid = TUid::Uid( aInfo.iUid.iUid );
        iName.Copy( aInfo.iName );
        iNamespace.Copy( aInfo.iNamespace );   
        return *this;  
        }
        
    inline TBool operator== (const TAiPublisherInfo& aInfo) const
        {
        if( iUid == aInfo.iUid && 
            iName == aInfo.iName &&
            iNamespace == aInfo.iNamespace )
            {
            return ETrue;
            }
            
        return EFalse;                                       
        }
    
    TUid                  iUid;
    TAiPublisherName      iName;
    TAiPublisherNamespace iNamespace;
    };

/**
 * Defines set of properties supported by plug-in.
 *
 * Example how to provide and set the properties.
 * @code
 * void CMyAiPlugin::ConstructL()
 *     {
 *     iInfo = new (ELeave) TAiPublisherInfo;
 *     iContent = AiUtility::CreateContentItemArrayIteratorL( KMyAiPluginContent );
 *     iResources = AiUtility::CreateContentItemArrayIteratorL( KMyAiPluginResources );
 *     iEvents = AiUtility::CreateContentItemArrayIteratorL( KMyAiPluginEvents );
 *     }
 *
 * void CMyAiPlugin::SetPropertyL( TInt aProperty, TAny* aValue )
 *     {
 *     if( !aValue ) return;
 * 
 *     switch( aProperty )
 *         {
 *         case EAiPublisherInfo:
 *             {
 *             const TAiPublisherInfo* info =
 *                 static_cast<const TAiPublisherInfo*>(aValue);
 *             iInfo->iUid.iUid = info->iUid.iUid;
 *             iInfo->iName.Copy( info->iName );
 *             break;
 *             }
 *         }
 *      }
 *
 * TAny* CMyAiPlugin::GetPropertyL( TInt aProperty )
 *     {
 *     switch( aProperty )
 *         {
 *         case EAiPublisherInfo:
 *             return iInfo;
 * 
 *         case EAiPublisherContent:
 *             return iContent;
 * 
 *         case EAiPublisherResources:
 *             return iResources;
 * 
 *         case EAiPublisherEvents:
 *             return iEvents;
 *         }
 *     return NULL;
 *     }
 * @endcode
 */
enum TAiPublisherProperty
    {
    /**
     * Enables read-only access to Plug-in information. GetProperty MUST return
     * instance of struct TAiPublisherInfo via TAny* .
     */
    EAiPublisherInfo = 0x0001,

    /**
     * Enables read-only access to iterator of content selectors. GetProperty
     * MUST return instance of MAiContentItemIterator for content selectors.
     */
    EAiPublisherContent,

    /**
     * Enables read-only access to iterator of content references. GetProperty
     * MUST return instance of MAiContentItemIterator for content references.
     */
    EAiPublisherResources,

    /**
     * Enables read-only access to iterator of events supported by plug-in.
     * GetProperty MUST return instance of MAiContentItemIterator for events.
     */
    EAiPublisherEvents,

    /**
     * Provides access to MAiContentRequest interface for refreshing a content
     * item.
     * @see EAiPublisherContent
     */
    EAiContentRequest,

    /**
     * Provides access to MAiContentRequest interface for refreshing a resource
     * item.
     * @see EAiPublisherResources
     */
    EAiResourceRequest,

    /**
     * Provides access to localized plugin name if supported. HBufC*
     * @see EAiPublisherResources
     */
    EAiPluginName
    };


/**
 * Property extension interface for CAiContentPublisher.
 *
 * @see CAiContentPublisher::Extension
 * @since S60 3.2
 */
class MAiPropertyExtension
    {
public:  // New functions

    /**
     * Read property of publisher plug-in.
     *
     * @param aProperty - identification of property.
     * @return Pointer to property value.
     */
    virtual TAny* GetPropertyL(TInt aProperty) = 0;

    /**
     * Helper function for accessing the Publisher Info Property.
     */
    inline const TAiPublisherInfo* PublisherInfoL();

    /**
     * Write property value.
     *
     * @param aProperty - identification of property.
     * @param aValue - contains pointer to property value.
     */
    virtual void SetPropertyL(TInt aProperty, TAny* aValue) = 0;

protected:
    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiPropertyExtension() { }
    };

#include <aipropertyextension.inl>

#endif  // M_AIPROPERTYEXTENSION_H
