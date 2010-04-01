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
* Description:  AI Shortcut xSP Extension API
*
*/


#include <e32base.h>
#include <s32mem.h>
#include <gulicon.h>
#include <fbs.h>

#include <aiscutextserv.h>
#include <aiscutextdefs.h>

// ======== LOCAL DEFINITIONS ========

namespace
    {
    //  LOCAL CONSTANTS
    /**
     * Default message slots
     */
    const TUint KDefaultMessageSlots = 4;

    /**
     * Marshalling buffer expand size
     */
    const TInt KBufExpandSize = 32;

    /**
     * Target string format that matches with aiscutplugin
     */
    _LIT( KTargetStringFormat, "localapp:0x%x" );

    //  LOCAL TYPES
    typedef TBuf<19> TTargetString;

    //  LOCAL FUNCTIONS
    /**
     * Panics server
     * @aReason Panic reason code
     */
    void Panic( TInt aReason )
        {
        User::Panic( KAiScutExtServerName, aReason );
        }

    /**
     * Generates target string from Uid3 of current process
     */
    TTargetString DefaultTargetString()
        {
        TUid uid3 = RProcess().Type()[ 2 ];
        TTargetString targetString;
        targetString.Format( KTargetStringFormat, uid3 );
        return targetString;
        }
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::Connect()
    {
    return Connect( DefaultTargetString() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::Connect( const TDesC& aTargetDefinition )
    {
    TInt err = CreateSession( KAiScutExtServerName, Version(),
        KDefaultMessageSlots );
    if( err == KErrNone )
        {
        TIpcArgs args( &aTargetDefinition );
        err = SendReceive( EAiScutExtServSetTargetDefinition, args );
        if( err != KErrNone )
            {
            Close();
            }
        }
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TVersion RAiScutExtServ::Version() const
    {
    return( TVersion(
        KAiScutExtServMajorVersionNumber,
        KAiScutExtServMinorVersionNumber,
        KAiScutExtServBuildVersionNumber ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::UpdatePopupTextL(
    const MDesCArray& aPopupTextLines )
    {
    TInt lineCount = aPopupTextLines.MdcaCount();
    __ASSERT_ALWAYS( lineCount <= KMaxPopupTextLines, Panic( KErrArgument ) );
    CBufBase* lineArrayBuf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( lineArrayBuf );

    RBufWriteStream stream( *lineArrayBuf );

    stream.WriteUint8L( lineCount );
    for( TInt i = 0; i < lineCount; i++ )
        {
        stream << aPopupTextLines.MdcaPoint( i );
        }

    TPtr8 lineArray = lineArrayBuf->Ptr( 0 );

    TIpcArgs args( &lineArray );
    TInt err = SendReceive( EAiScutExtServSetPopupText, args );

    CleanupStack::PopAndDestroy( lineArrayBuf );
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::ResetPopupText()
    {
    return SendReceive( EAiScutExtServResetPopupText );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::UpdateIconL( const CGulIcon& aIcon )
    {
    CBufBase* marshallBuf = CBufFlat::NewL( KBufExpandSize );
    CleanupStack::PushL( marshallBuf );

    RBufWriteStream stream( *marshallBuf );

    aIcon.Bitmap()->ExternalizeL( stream );
    aIcon.Mask()->ExternalizeL( stream );

    TPtr8 marshalledData = marshallBuf->Ptr( 0 );

    TIpcArgs args( &marshalledData );
    TInt err = SendReceive( EAiScutExtServSetIcon, args );

    CleanupStack::PopAndDestroy( marshallBuf );
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::ResetIcon()
    {
    return SendReceive( EAiScutExtServResetIcon );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::IsInShortcuts( TBool& aIsInShortcuts ) const
    {
    TPtr8 isInShortcutsDes(
        reinterpret_cast< TUint8* >( &aIsInShortcuts ),
        sizeof( aIsInShortcuts ),
        sizeof( aIsInShortcuts ) );

    TIpcArgs args( &isInShortcutsDes );

    return SendReceive( EAiScutExtServIsInShortcuts, args );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RAiScutExtServ::IssuePutInShortcuts()
    {
    return SendReceive( EAiScutExtServIssuePutInShortcuts );
    }

// End of File.
