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
* Description:  Active Idle content observer interface.
*
*/


#ifndef M_AICONTENTOBSERVER_H
#define M_AICONTENTOBSERVER_H

// System includes
#include <e32std.h>

// User includes

// Forward declarations
class CHsContentPublisher;
class THsPublisherInfo;
class RFile;

/**
 *  Used by AI Plug-in to give notification about modifications in
 *  content and/or internal state.
 *
 *  @since S60 3.2
 */
class MAiContentObserver
    {
public: 
    // data types
    
    /**
    * Value type for SetProperty
    *
    * @since S60 5.2
    */
    enum TValueType
        {
        EValueUnknown = 0,
        EValueNumber,
        EValuePercentage,
        EValuePx,
        EValueDimension,
        EValueString,
        EValueIdent,
        EValueAttr,
        EValueCounter,
        EValueRect,
        EValueRgbColor,
        EValueRgbaColor,
        EValueFunction,
        EValueUnitValue
        };
    
public:  
    // new functions

    /**
     * Invoked by the plug-in to inform that it initiates content publishing
     * transaction.
     *
     * @param  aTxId - transaction Id
     * @return - Possible return values:
     *         - KErrNone - transaction is started.
     *         - KErrNotSupported - UI Controller does not support transactions,
     *         meaning that content will be rendered every time when Publish
     *         is called.
     */
    virtual TInt StartTransaction( TInt aTxId ) = 0;

    /**
     * Invoked by the plug-in to request framework that content publishing
     * transaction must be finalized and content should be rendered to the
     * screen.
     *
     * @param  aTxId - transaction Id
     * @return - Possible return values:
     *         - KErrNone - transaction is over.
     *         - KErrNotSupported - UI Controller does not support transactions,
     *         meaning that content will be rendered every time when Publish
     *         is called.
     *         - KErrNotFound - transaction with matching id has not been 
     *         started.
     *         - Any of the system-wide error codes in case of resource allocation failure.
     */
    virtual TInt Commit( TInt aTxId ) = 0;

    /**
     * Invoked by plug-in to indicate that content publishing transaction
     * must be cancelled.
     *
     * @param  aTxId - transaction Id
     * @return - Possible return values:
     *         - KErrNone - transaction is cancelled.
     *         - KErrNotSupported - UI Controller does not support transactions,
     *         meaning that content will be rendered every time when Publish
     *         is called.
     *         - KErrNotFound - transaction with matching id has not been 
     *         started.
     */
    virtual TInt CancelTransaction( TInt aTxId ) = 0;

    /**
     * Invoked by plug-in to test if the specified content can be published.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aIndex - index of the content item.
     * @return ETrue - if content could be published; EFalse otherwise.
     */
    virtual TBool CanPublish( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex ) = 0;

    /**
     * Invoked by the plug-in to inform that content identified by reference
     * aResource must be published to UI control\element identified by selector
     * aContent.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aResource - identification of content reference, MUST correspond
     *         single content reference supported by plug-in. The framework
     *         utilizes the reference if to match for cid and MIME type of the
     *         content supplied with in UI definition.
     * @param  aIndex - index of the content item.
     * @return - Possible return values:
     *         - KErrNone - if content is published.
     *         - KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         - KErrNotFound - if content reference is not found in current
     *         UI definition.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, TInt aResource, TInt aIndex ) = 0;

    /**
     * Invoked by the plug-in to inform that textual content provided within 
     * parameter aText must be published to UI control\element identified by 
     * selector aContent.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aText - Textual content in UNICODE.
     * @param  aIndex - index of the content item.
     * @return - Possible return values:
     *         - KErrNone - if content is published.
     *         - KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         - KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         - KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, const TDesC16& aText, TInt aIndex ) = 0;

    /**
     * Invoked by the plug-in to inform that content provided within buffer
     * aBuf must be published to UI control\element identified by selector
     * aContent.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aBuf - instance of content.
     * @param  aIndex - index of the content item.
     * @return - Possible return values:
     *         - KErrNone - if content is published.
     *         - KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         - KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         - KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     */ 
    virtual TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, const TDesC8& aBuf, TInt aIndex ) = 0;
    
    /**
     * Invoked by the plug-in to inform that an object provided by pointer aPtr
     * must be published to UI control\element identified by selector aContent.
     * The implementation packages the pointer to a buffer and delegates to 
     * buffer publishing method Publish(MAiPropertyExtension&, TInt, const TDesC8&, TInt).
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aPtr - pointer to the content object. The actual type of the 
     *         pointer is deduced based on the MIME type of the content item 
     *         identified by aContent.
     * @param  aIndex - index of the content item.
     * @return - Possible return values:
     *         - KErrNone - if content is published.
     *         - KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         - KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         - KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     * @see KAiContentTypeBitmap
     */ 
    inline TInt PublishPtr( CHsContentPublisher& aPlugin, TInt aContent, TAny* aPtr, TInt aIndex );

    /**
     * Helper function for unpacking a pointer that has been published with
     * MAiContentObserver::PublishPtr.
     *
     * @see MAiContentObserver::PublishPtr
     */
    template<class PtrT> inline static PtrT* UnpackPtr( const TDesC8& aBuf );

    /**
     * Invoked by the plug-in to inform that content from file handle aFile
     * must be published to UI control\element identified by selector aContent.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aFile - file handle from where content can be obtained by UI
     *         framework.
     * @param  aIndex - index of the content item.
     * @return - Possible return values:
     *         - KErrNone - if content is published.
     *         - KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         - KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         - KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     */
    virtual TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, RFile& aFile, TInt aIndex ) = 0;

    /**
     * Invoked by the plug-in to inform that content must be cleaned in UI
     * control\element identified by selector aContent.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aIndex - index of the content item.
     * @return - Possible return values:
     *         - KErrNone - if content is published.
     *         - KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         - KErrNotFound - if content reference is not found in current
     *         UI definition.
     */
    virtual TInt Clean( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex ) = 0;

    /**
     * Returns interface extension. Not used in S60 3.2 release.
     *
     * @param  aUid - UID of the extension interface to access.
     * @return the extension interface. Actual type depends on the passed aUid 
     *         argument.
     */
    virtual TAny* Extension( TUid aUid ) = 0;

    /**
     * Invoked by the plugin factory
     *
     * @param aPublsiherInfo Publisher which requires subscription
     * @return ETrue if subsription is needed, EFalse otherwise
     */ 
    virtual TBool RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const = 0;     
    
    /**
     * Invoked by the plug-in to change the property value of a specific content.
     * value type must be string.
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aElementId - id of content selector, MUST correspond
     *         single content supported by plug-in. The framework
     *         utilizes the id to find in the plugin xml defintion.
     * @param  aPropertyName - property name.
     * @param  aPropertyValue - property value.
     * @return - Possible return values:
     *         - KErrNone - if content reference found.
     *         - KErrNotFound - if content reference is not found in current
     *         plugin definition.
     *         - KErrNotSupported - if content selector is not supported by
     *         current plugin definition.         
     */
    virtual TInt SetProperty( CHsContentPublisher& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue ) = 0;
    
    /**
     * Invoked by the plug-in to change the property value of a specific content.
     *
     * @param  aPlugin - Plug-in interface implementation.
     * @param  aElementId - id of content selector, MUST correspond
     *         single content supported by plug-in. The framework
     *         utilizes the id to find in the plugin xml defintion.
     * @param  aPropertyName - property name.
     * @param  aPropertyValue - property value.
     * @param  aValueType  - value type.
     * @return - Possible return values:
     *         - KErrNone - if content reference found.
     *         - KErrNotFound - if content reference is not found in current
     *         plugin definition.
     *         - KErrNotSupported - if content selector is not supported by
     *         current plugin definition.         
     */
    virtual TInt SetProperty( CHsContentPublisher& aPlugin,
            const TDesC8& aElementId,
            const TDesC8& aPropertyName,
            const TDesC8& aPropertyValue,  
            MAiContentObserver::TValueType aValueType) = 0;
            
protected:
    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiContentObserver() { }
    };


#include <aicontentobserver.inl>

#endif // M_AICONTENTOBSERVER_H
