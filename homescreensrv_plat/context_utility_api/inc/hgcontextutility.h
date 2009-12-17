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


#ifndef HGCONTEXTUTILITY_H
#define HGCONTEXTUTILITY_H

#include <e32base.h>
#include <e32cmn.h>
#include <bamdesca.h>
#include <badesca.h>
#include <mdccommon.h>
#include <hg/hgcontextutilitybase.h>

class MVPbkStoreContact;
class MVPbkContactLink;
class CVPbkContactLinkArray;
class CMdESession;

/**
 *  Utility class to publish and access context through the Context Framework.
 *  @lib hgcontextutility.lib
 *
 *  Typical usage in applications that are publishing context:
 *  During construction:
 *  \code
 *    iContextUtility = CHgContextUtility::NewL();
 *    iContextUtility->RePublishWhenFgL( ETrue );
 *  \endcode
 *  Publishing is accomplished with one single call, for example:
 *  \code
 *    void ContactChanged( MVPbkStoreContact* aNewContact ) {
 *      ...
 *      iContextUtility->PublishContactContextL( *aNewContact );
 *      ...
 *  \endcode
 *
 *  By default publishing requests are ignored if the application is
 *  not in foreground (does not apply to apps that do not have the
 *  ui environment). If there is still a good reason to allow this
 *  then call AllowPublishFromBackground( ETrue ).
 */
NONSHARABLE_CLASS( CHgContextUtility ) : public CHgContextUtilityBase
    {
public:
    /**
     * 2-phased constructor.
     */
    IMPORT_C static CHgContextUtility* NewL();
    
    /**
     * 2-phased constructor.
     */
    IMPORT_C static CHgContextUtility* NewLC();
    
    /*
     * Destructor.
     */
    IMPORT_C ~CHgContextUtility();

    /**
     * Publishes contact context.
     * @param   aContact            contact
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishContactContextL( const MVPbkStoreContact& aContact,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Publishes contact context.
     * @param   aContactLink        contact link
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishContactContextL( const MVPbkContactLink& aContactLink,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

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
    IMPORT_C void PublishContactContextL( const TDesC& aContactName,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Overload for publishing multiple contacts.
     * Ownership of passed pointers is not taken.
     * @param   aContacts           contact array
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishContactContextL(
        const RPointerArray<MVPbkStoreContact>& aContacts,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Overload for publishing multiple contacts.
     * @param   aContactLinks       contact link array
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishContactContextL(
        const CVPbkContactLinkArray& aContactLinks,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Overload for publishing multiple contacts.
     * @param   aContactNames   string array, for element format
     * see PublishContactContextL(const TDesC&)
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishContactContextL( const MDesCArray& aContactNames,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Publishes freetext context.
     * Not to be used for bulk text (e.g. full content of some text viewer component).
     * @param   aText   some text, typically the highlighted substring
     * from a viewer or editor control
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishTextContextL( const TDesC& aText,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Publishes URL context.
     * @param   aUrl    URL
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishUrlContextL( const TDesC& aUrl,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Publishes date/time context.
     * @param   aTime   time
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishTimeContextL( const TTime& aTime,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Publishes photo context.
     * @param   aFilename   name of image file, with full path
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishPhotoContextL( const TDesC& aFilename,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );

    /**
     * Publishes photo context.
     * @param   aMdeItemId  item id for the Image object in MDS
     * @param   aMdeSession opened metadata engine session
     * @param   aDelay              if non-zero then context is published only after
     * a short delay. If a new publish call is made before the timer fires the
     * pending value will not be published.
     */
    IMPORT_C void PublishPhotoContextL( TItemId aMdeItemId,
        CMdESession& aMdeSession,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );
    
    /**
     * Publishes TV context.
     * Pass KNullDesC if some of the needed data is not available.
     * @param   aChannelName        channel name
     * @param   aProgramName        program name
     * @param   aProgramDescription program description
     * @param   aGenre              genre
     */
    IMPORT_C void PublishTvContextL(
        const TDesC& aChannelName,
        const TDesC& aProgramName,
        const TDesC& aProgramDescription,
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
    IMPORT_C void PublishServiceIdL( const TDesC& aServiceId,
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
    IMPORT_C void RePublishWhenFgL( TBool aEnable );

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
    IMPORT_C void AllowPublishFromBackground( TBool aAllow );
        
    /**
     * Adds new music context info to music context publishing parameters.
     * This method is to be used in context with PublishMusicContextL. Fill in the
     * parameters and then call PublishMusicContextL.
     * This parameter list is cleared after publishing music context.
     * 
     * @param aKey Name of the key to be published.
     * @param aData Data for the key.
     * @leave KErrNotFound, when key is empty.
     * @leave KErrAlreadyExists, when key is already in the list.
     * @see hgcontextdef.h for keys
     * @see PublishMusicContextL
     */
    IMPORT_C void AddMusicContextInfoL( const TDesC& aKey, const TDesC& aData );
    
    /**
     * Publishes music context from provided parameters as music context.
     * Publishes all known (in hgcontextdef.h) music context fields as music
     * context. All keys, which do not contain any data, are automatically 
     * filled with '<unknown>' -string.
     * Clears the parameter list after publishing the context.
     * 
     * @leave KErrNotReady, when music context data parameter list is empty.
     * * @see hgcontextdef.h for keys
     * @see AddMusicContextInfoL
     */
    IMPORT_C void PublishMusicContextL( 
        const TTimeIntervalMicroSeconds32& aDelay = 0 );
    
    /**
     * Publishes Radio context.
     * Pass KNullDesC if some of the needed data is not available.
     * @param   aRadioName        radio name
     * @param   aRadioUrl         radio url
     * @param   aRadioFrequency   radio frequency
     * @param   aRadioRDSPI       radio identification code
     */
    IMPORT_C void PublishRadioContextL( 
            const TDesC& aRadioName,
            const TDesC& aRadioUrl,
            const TDesC& aRadioFrequency,
            const TDesC& aRadioRDSPI );
    
    /**
     * Creates a combined string from the elements of the given array.
     * Returns NULL if the array has less than 2 elements.
     * @param   aArray      string array
     * @return  combined string or NULL, ownership transferred to caller
     */
    IMPORT_C static HBufC* BuildCombinedStringL( const MDesCArray& aArray );

    /**
     * Splits the given combined string and appends the components to
     * the given array. The initial content of the array is not modified.
     * If aString does not seem to be a string combined from multiple entries
     * then aString is appended to aArray without any changes.
     * @param   aString     combined string, input
     * @param   aArray      array, output
     */
    IMPORT_C static void SplitCombinedStringL( const TDesC& aString,
        CDesCArray& aArray );

protected:
    CHgContextUtility();
    void ConstructL();
    };
    
#endif /* HGCONTEXTUTILITY_H */
