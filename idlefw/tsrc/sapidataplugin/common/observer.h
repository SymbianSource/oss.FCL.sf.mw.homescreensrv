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
* Description:  
*
*/

#ifndef __OBSERVER_H__
#define __OBSERVER_H__

// External includes
#include <e32base.h>
#include <aicontentobserver.h>
#include <hscontentpublisher.h>

class RFile;

/**
 * Observer class
 */
class Observer : public CBase, 
    public MAiContentObserver
    {
public:

    static Observer* NewL();

    virtual ~Observer();

    /**
     * Invoked by the plug-in to inform that it initiates content publishing
     * transaction.
     *
     * @param  aTxId - transaction Id
     * @return KErrNone - transaction is started.
     *         KErrNotSupported - UI Controller does not support transactions,
     *         meaning that content will be rendered every time when Publish
     *         is called.
     * @since S60 v3.2
     */
    TInt StartTransaction( TInt aTxId );

	inline TInt CancelTransaction( TInt /*aTxId*/ )
		{
		return 10000000000;
		};

    /**
     * Invoked by the plug-in to request framework that content publishing
     * transaction must be finalized and content should be rendered to the
     * screen.
     *
     * @param  aTxId - transaction Id
     * @return KErrNone - transaction is over.
     *         KErrNotSupported - UI Controller does not support transactions,
     *         meaning that content will be rendered every time when Publish
     *         is called.
     *         KErrNotFound - transaction with matching id has not been
     *         started.
     * @since S60 v3.2
     */
    TInt Commit( TInt aTxId );

    /**
     * Invoked by plug-in to indicate that content publishing transaction
     * must be cancelled.
     *
     * @param  aTxId - transaction Id
     * @return KErrNone - transaction is cancelled.
     *         KErrNotSupported - UI Controller does not support transactions,
     *         meaning that content will be rendered every time when Publish
     *         is called.
     *         KErrNotFound - transaction with matching id has not been
     *         started.
     * @since S60 v3.2
     */
    TInt Rollback( TInt aTxId );

    /**
     * Invoked by plug-in to test if content can be published.
     *
     * @param  aPlugin - Plug-in property extension interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aIndex - index of the content item.
     * @return ETrue - if content could be published.
     * @since  S60 3.2
     */
    TBool CanPublish( CHsContentPublisher& aPlugin, TInt aContent, 
        TInt aIndex );

    /**
     * Invoked by the plug-in to inform that content identified by reference
     * aResource must be published to UI control\element identified by selector
     * aContent.
     *
     * @param  aPlugin - Plug-in property extension interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aResource - identification of content reference, MUST correspond
     *         single content reference supported by plug-in. The framework
     *         utilizes the reference if to match for cid and MIME type of the
     *         content supplied with in UI definition.
     * @param  aIndex - index of the content item.
     * @return KErrNone - if content is published.
     *         KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         KErrNotFound - if content reference is not found in current
     *         UI definition.
     * @since S60 v3.2
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, 
        TInt aResource, TInt aIndex );

    /**
     * Invoked by the plug-in to inform that textual content provided within
     * parameter aText must be published to UI control\element identified by
     * selector aContent.
     *
     * @param  aPlugin - Plug-in property extension interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aText - Textual content in UNICODE.
     * @param  aIndex - index of the content item.
     * @return KErrNone - if content is published.
     *         KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     * @since S60 v3.2
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, 
        const TDesC16& aText, TInt aIndex );

    /**
     * Invoked by the plug-in to inform that content provided within buffer
     * aBuf must be published to UI control\element identified by selector
     * aContent.
     *
     * @param  aPlugin - Plug-in property extension interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aBuf - instance of content.
     * @param  aIndex - index of the content item.
     * @return KErrNone - if content is published.
     *         KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     * @since S60 v3.2
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, 
        const TDesC8& aBuf, TInt aIndex );

    /**
     * Invoked by the plug-in to inform that content from file handle aFile
     * must be published to UI control\element identified by selector aContent.
     *
     * @param  aPlugin - Plug-in property extension interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aFile - file handle from where content can be obtained by UI
     *         framework.
     * @param  aIndex - index of the content item.
     * @return KErrNone - if content is published.
     *         KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         KErrNotFound - if content reference is not found in current
     *         UI definition.
     *         KErrArgument - if content cannot be published to UI element,
     *         e.g. MIME type mismatch.
     * @since S60 v3.2
     */
    TInt Publish( CHsContentPublisher& aPlugin, TInt aContent, 
        RFile& aFile, TInt aIndex );

    /**
     * Invoked by the plug-in to inform that content must be cleaned in UI
     * control\element identified by selector aContent.
     *
     * @param  aPlugin - Plug-in property extension interface implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aIndex - index of the content item.
     * @return KErrNone - if content is published.
     *         KErrNotSupported - if content selector is not supported by
     *         current UI definition.
     *         KErrNotFound - if content reference is not found in current
     *         UI definition.
     * @since S60 v3.2
     */
    TInt Clean( CHsContentPublisher& aPlugin, TInt aContent, TInt aIndex );

    /**
     * Returns interface extension. Not used in Series 60 3.1 release.
     *
     * @param  aUid - UID of the extension interface to access.
     * @return the extension interface. Actual type depends on the passed aUid
     *         argument.
     * @since S60 v3.2
     */
    TAny* Extension( TUid aUid ) ;
    
    /**
     *
     */
    TBool RequiresSubscription( const THsPublisherInfo& aPublisherInfo ) const;  
    
    /**
     *
     */
    TInt SetProperty( CHsContentPublisher& aPlugin,
        const TDesC8& aElementId,
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue );
    
    /**
     *
     */
    TInt SetProperty( CHsContentPublisher& aPlugin,
        const TDesC8& aElementId,
        const TDesC8& aPropertyName,
        const TDesC8& aPropertyValue,  
        MAiContentObserver::TValueType aValueType );        

protected:

private:

    Observer();

    void ConstructL();

    /**
     * Leaving version of the commit operation
     *
     * @since S60 3.1
     */
    void DoCommitL( TInt aTxId );

private:    // Data

    /**
     * Transaction id
     */
    TInt iTransactionId;

    /**
     * Flag indicating if transaction is ongoing
     */
    TBool iTransactionOngoing;
    };

#endif // __OBSERVER_H__

// End of File.
