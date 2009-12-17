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
* Description:  Context publishing helper dll
*
*/


#ifndef HGCONTEXTUTILITYIMPL_H
#define HGCONTEXTUTILITYIMPL_H

#include <e32base.h>
#include <cflistener.h>
#include <bamdesca.h>
#include <badesca.h>
#include <mdccommon.h>
#include <coemain.h>
#include <e32hashtab.h>

class CCFClient;
class MVPbkContactLink;
class MVPbkStoreContact;
class CVPbkContactLinkArray;
class CMdESession;

/**
 *  Utility class offering static and non-static functions for
 *  publishing and accessing context through the Context Framework.
 */
NONSHARABLE_CLASS( CHgContextUtilityImpl ) :
        public CTimer,
        public MCFListener,
        public MCoeForegroundObserver
    {
public:
    static CHgContextUtilityImpl* NewL();
    static CHgContextUtilityImpl* NewLC();
    virtual ~CHgContextUtilityImpl();
    
    /**
     * Publishes context.
     * Also defines the context if it has not been defined.
     * Publishing empty value is not allowed, however such errors are ignored
     * here so the function will not leave when CFW responds with KErrArgument.
     * The security policy for the context will be set to require
     * LocalServices capability.
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context
     */
    void PublishContextL( const TDesC& aContextType,
        const TDesC& aContextData );

    /**
     * Publishes context, the value will contain all the strings
     * from the given array, typically by using some separator character.
     * @see PublishContextL
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context will be a combined
     * version of all the strings from this array
     */
    void PublishContextL( const TDesC& aContextType,
        const MDesCArray& aContextData );
    
    /**
     * Static version that uses the given CCFClient instance.
     * @param   aCFClient       a CCFClient instance
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context
     */
    static void PublishContextL( CCFClient& aCFClient,
        const TDesC& aContextType, const TDesC& aContextData );

    /**
     * Publishes context but only after a short interval, using a timer.
     * If it is called again before the timer expires then the timer
     * is restarted (and so the previous pending value is never published).
     * @param   aContextType    context type, source is always KHgCFSource
     * @param   aContextData    value for the context
     * @param   aDelay          delay for the timer
     */
    void PublishContextDelayedL( const TDesC& aContextType,
        const TDesC& aContextData, TTimeIntervalMicroSeconds32 aDelay );

    /**
     * Overload for delayed publishing of a value combined from multiple strings.
     * @param   aContextType    context type
     * @param   aContextData    string array
     * @param   aDelay          delay for the timer, in microseconds
     */
    void PublishContextDelayedL( const TDesC& aContextType,
        const MDesCArray& aContextData, TTimeIntervalMicroSeconds32 aDelay );

    /**
     * Requests the given context and returns the value for the
     * first result. Returns NULL if not found.
     * @param   aContextType    context type, the source is always KHgCFSource
     */        
    HBufC* GetContextL( const TDesC& aContextType );
    
    /**
     * Requests the given context and returns the value for the
     * first result. Returns NULL if not found.
     * @param   aContextSource  context source
     * @param   aContextType    context type
     */
    HBufC* GetContextL( const TDesC& aContextSource,
        const TDesC& aContextType );

    /**
     * Creates a combined string from the elements of the given array.
     * Returns NULL if the array has less than 2 elements.
     * @param   aArray      string array
     * @return  combined string or NULL, ownership transferred to caller
     */
    static HBufC* BuildCombinedStringL( const MDesCArray& aArray );

    /**
     * Splits the given combined string and appends the components to
     * the given array. The initial content of the array is not modified.
     * If aString does not seem to be a string combined from multiple entries
     * then aString is appended to aArray without any changes.
     * @param   aString     combined string, input
     * @param   aArray      array, output
     */
    static void SplitCombinedStringL( const TDesC& aString,
        CDesCArray& aArray );

    /**
     * Gets a string that can be published straight via PublishContextL.
     * The pointer is left on the cleanup stack.
     * Returned & pushed pointer may also be NULL is something goes wrong.
     */
    HBufC* MakeLinkPublishableLC( const MVPbkContactLink& aLink );

    /**
     * Publishes contact context.
     * @param   aContact            contact
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishContactContextL( const MVPbkStoreContact& aContact,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes contact context.
     * This may include async operations and therefore the actual publishing may happen
     * only after this function returns.
     * @param   aContactLink        contact link
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishContactContextL( const MVPbkContactLink& aContactLink,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes contact context.
     * Attempts to publish an empty value will be ignored.
     *
     * Prefer using the overloads taking vpbk contact or link, whenever possible.
     * Use this in case of unknown contacts only, that is, a name, phone number,
     * or email address that does not belong to a phonebook contact.
     *
     * @param   aContactName    formatted name, phone number, or email address,
     * or a combination of them, e.g. "firstname lastname", "+12345678",
     * "lastname firstname <abcd@efg.com>", "firstname, lastname <0501234567>".
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishContactContextL( const TDesC& aContactName,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Overload for publishing multiple contacts.
     * @param   aContacts           contact array
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishContactContextL(
        const RPointerArray<MVPbkStoreContact>& aContacts,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Overload for publishing multiple contacts.
     * This may include async operations and therefore the actual publishing may happen
     * only after this function returns.
     * @param   aContactLinks       contact link array
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishContactContextL(
        const CVPbkContactLinkArray& aContactLinks,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Overload for publishing multiple contacts.
     * @param   aContactNames   string array, for element format
     * see PublishContactContextL(const TDesC&)
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishContactContextL( const MDesCArray& aContactNames,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes freetext context.
     * Not to be used for bulk text (e.g. full content of some text viewer component).
     * @param   aText   some text, typically the highlighted substring
     * from a viewer or editor control
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishTextContextL( const TDesC& aText,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes URL context.
     * @param   aUrl    URL
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishUrlContextL( const TDesC& aUrl,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes date/time context.
     * @param   aTime   time
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishTimeContextL( const TTime& aTime,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes photo context.
     * @param   aFilename   name of image file, with full path
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishPhotoContextL( const TDesC& aFilename,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes photo context.
     * @param   aMdeItemId  item id for the Image object in MDS
     * @param   aMdeSession opened metadata engine session
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishPhotoContextL( TItemId aMdeItemId,
        CMdESession& aMdeSession,
        const TTimeIntervalMicroSeconds32& aDelay );

    /**
     * Publishes TV context.
     * Pass KNullDesC if some of the needed data is not available.
     * @param   aChannelName        channel name
     * @param   aProgramName        program name
     * @param   aProgramDescription program description
     * @param   aGenre              genre
     */
    void PublishTvContextL( const TDesC& aChannelName,
        const TDesC& aProgramName, const TDesC& aProgramDescription,
        const TDesC& aGenre );

    /**
     * Publishes an account id as contact context.
     *
     * @param   aServiceId  the service prefix, e.g. "Ovi"
     * @param   aAccountId  the account id
     * @param   aDelay   if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    void PublishServiceIdL( const TDesC& aServiceId,
        const TDesC& aAccountId,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Enables or disables automatic re-publishing of the latest
     * context (published via this context utility instance) whenever
     * the application comes to foreground.
     *
     * It is DISABLED by default.
     *
     * By enabling this the applications do not have to care about
     * context publishing in HandleForegroundEventL etc.
     *
     * The feature needs CCoeEnv and calls will be ignored if the
     * environment is not available.
     *
     * @param   aEnable     flag to turn the feature on/off
     */
    void RePublishWhenFgL( TBool aEnable );

    /**
     * Enables or disables context publishing when being in background.
     * Applies to all PublishContextL variants.
     * If disabled then no context will be published if it seems that the
     * caller application is not in foreground.
     * Has no effect if there is no CCoeEnv available, publishing is always
     * allowed in that case.
     *
     * It is DISABLED by default, that is, publishing is not allowed
     * from applications that are not in foreground.
     *
     * @param   aAllow  flag to turn the feature on/off
     */
    void AllowPublishFromBackground( TBool aAllow );
    
    /**
     * @see CHgContextUtility::AddMusicContextInfo
     */
    void AddMusicContextInfoL( const TDesC& aKey, const TDesC& aData );
    
    /**
     * @see CHgContextUtility::AddMusicContextInfo
     */
    void PublishMusicContextL( 
        const TTimeIntervalMicroSeconds32& aDelay = 0 );
    
    /**
     * @see CHgContextUtility::PublishRadioContextL
     */
    void PublishRadioContextL( 
            const TDesC& aRadioName,
            const TDesC& aRadioUrl,
            const TDesC& aRadioFrequency,
            const TDesC& aRadioRDSPI );

private: // from MCFListener
    void ContextIndicationL( const CCFContextIndication& aChangedContext );
    void ActionIndicationL( const CCFActionIndication& aActionToExecute );
    void HandleContextFrameworkError( TCFError aError,
        const TDesC& aSource, const TDesC& aType );
    TAny* Extension( const TUid& aExtensionUid ) const;

private: // from MCoeForegroundObserver
    void HandleGainingForeground();
    void HandleLosingForeground();

private: // from CTimer
    void RunL();
    TInt RunError( TInt aError );

private:
    /**
     * Constructor.
     */
    CHgContextUtilityImpl();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Creates CCFClient instance if not yet done.
     * Does not leave if creation fails.
     * Returns ETrue if iCFClient is usable.
     */    
    TBool CFReady();
    
    /**
     * Returns ETrue if the root window's wgid is same as
     * the focused window group's wgid.
     * Always returns ETrue if CCoeEnv is not available.
     */
    TBool IsForeground();
    
    /**
     * Returns ETrue if publishing context is allowed at the moment.
     * Uses IsForeground and the allow-publish-from-background setting.
     */
    TBool AllowedToPublish();
    
    /**
     * Makes sure the specific key contains valid data and publishes it.
     * 
     * @param aKey Key to be checked and published.
     * @param aDelay Delay for publishing the context data. 
     */
    void VerifyAndPublishMusicContextL( 
        const TDesC& aKey,
        const TTimeIntervalMicroSeconds32& aDelay );
    
    /**
     * Simple wrapper to handle between delayed and instant publish.
     * @see PublishContextL
     * @see PublishContextDelayedL
     */
    void PublishContextL( 
        const TDesC& aContextType,
        const TDesC& aContextData,
        const TTimeIntervalMicroSeconds32& aDelay );

    CCFClient* iCFClient;
    HBufC* iPendingContextType;
    HBufC* iPendingContextData;
    CDesCArray* iPendingContextDataArray;

    HBufC* iLastContextType;
    HBufC* iLastContextData;
    TBool iFgWatchEnabled;
    CCoeEnv* iEnv; // not own
    TBool iAllowPublishFromBackground;
    
    /**
     * List of parameters to be published. Owns pointers.
     */
    RPtrHashMap<TDesC, TDesC> iMusicContextInfo;
    };
    
#endif /* HGCONTEXTUTILITYIMPL_H */
