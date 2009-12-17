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


#include <hg/hgcontextutility.h>
#include "hgcontextutilityimpl.h"

// -----------------------------------------------------------------------------
// CHgContextUtility::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CHgContextUtility* CHgContextUtility::NewL()
    {
    CHgContextUtility* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CHgContextUtility* CHgContextUtility::NewLC()
    {
    CHgContextUtility* self = new ( ELeave ) CHgContextUtility;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::CHgContextUtility
// -----------------------------------------------------------------------------
//
CHgContextUtility::CHgContextUtility()
    {
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::ConstructL
// -----------------------------------------------------------------------------
//
void CHgContextUtility::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::~CHgContextUtility
// -----------------------------------------------------------------------------
//
EXPORT_C CHgContextUtility::~CHgContextUtility()
    {
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishContactContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishContactContextL(
        const MVPbkStoreContact& aContact,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContactContextL( aContact, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishContactContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishContactContextL(
        const MVPbkContactLink& aContactLink,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContactContextL( aContactLink, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishContactContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishContactContextL(
        const TDesC& aContactName,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContactContextL( aContactName, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishContactContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishContactContextL(
        const RPointerArray<MVPbkStoreContact>& aContacts,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContactContextL( aContacts, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishContactContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishContactContextL(
        const CVPbkContactLinkArray& aContactLinks,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContactContextL( aContactLinks, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishContactContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishContactContextL(
        const MDesCArray& aContactNames,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishContactContextL( aContactNames, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishTextContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishTextContextL( const TDesC& aText,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishTextContextL( aText, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishUrlContextL
// -----------------------------------------------------------------------------
//   
EXPORT_C void CHgContextUtility::PublishUrlContextL( const TDesC& aUrl,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishUrlContextL( aUrl, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishTimeContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishTimeContextL( const TTime& aTime,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishTimeContextL( aTime, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishPhotoContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishPhotoContextL(
        const TDesC& aFilename,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishPhotoContextL( aFilename, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishPhotoContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishPhotoContextL(
        TItemId aMdeItemId,
        CMdESession& aMdeSession,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishPhotoContextL( aMdeItemId, aMdeSession, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishTvContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishTvContextL( const TDesC& aChannelName,
        const TDesC& aProgramName, const TDesC& aProgramDescription,
        const TDesC& aGenre )
    {
    iImpl->PublishTvContextL( aChannelName, aProgramName,
        aProgramDescription, aGenre );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishServiceIdL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishServiceIdL( const TDesC& aServiceId,
        const TDesC& aAccountId,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishServiceIdL( aServiceId, aAccountId, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::RePublishWhenFgL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::RePublishWhenFgL( TBool aEnable )
    {
    iImpl->RePublishWhenFgL( aEnable );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::AllowPublishFromBackground
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::AllowPublishFromBackground( TBool aAllow )
    {
    iImpl->AllowPublishFromBackground( aAllow );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::AddMusicContextInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::AddMusicContextInfoL( 
    const TDesC& aKey, const TDesC& aData )
    {
    iImpl->AddMusicContextInfoL( aKey, aData );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishMusicContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishMusicContextL( 
    const TTimeIntervalMicroSeconds32& aDelay )
    {
    iImpl->PublishMusicContextL( aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::PublishRadioContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CHgContextUtility::PublishRadioContextL( 
        const TDesC& aRadioName,
        const TDesC& aRadioUrl,
        const TDesC& aRadioFrequency,
        const TDesC& aRadioRDSPI )
    {
    iImpl->PublishRadioContextL( aRadioName, aRadioUrl,
            aRadioFrequency, aRadioRDSPI );
    }

// -----------------------------------------------------------------------------
// CHgContextUtility::BuildCombinedStringL
// -----------------------------------------------------------------------------
EXPORT_C HBufC* CHgContextUtility::BuildCombinedStringL( const MDesCArray& aArray )
{
    return CHgContextUtilityImpl::BuildCombinedStringL(aArray);
}

// -----------------------------------------------------------------------------
// CHgContextUtility::SplitCombinedStringL
// -----------------------------------------------------------------------------
EXPORT_C void CHgContextUtility::SplitCombinedStringL( const TDesC& aString,
        CDesCArray& aArray )
{
    CHgContextUtilityImpl::SplitCombinedStringL(aString, aArray);
}
//
// end of file
