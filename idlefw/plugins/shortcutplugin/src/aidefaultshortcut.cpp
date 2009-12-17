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
* Description:  Platform default shortcuts.
*
*/


#include "aidefaultshortcut.h"
#include "taiscutparser.h"
#include <centralrepository.h>
#include <activeidle2domaincrkeys.h>
#include <e32base.h>
#include <libc/stddef.h>  // For wchar_t

namespace {

       struct TDefaultShortcut
           {
           /**
            * Identifier
            */
           TInt id;

           /**
            * Key to identify this shortcut
            */
           TInt key;

           /**
            * Definition for this shortcut
            */
           const wchar_t* definition;
           };
       
       enum TDefaultShortcutCenrepId
           {
           EDefaultShortcutFirst = 0,
           EDefaultShortcutSecond,
           EDefaultShortcutThird,
           EDefaultShortcutFourth,
           EDefaultShortcutFifth,
           EDefaultShortcutSixth,
           EDefaultShortcutSeventh,
           EDefaultShortcutEighth,
           EDefaultShortcutNaviLeft,
           EDefaultShortcutNaviRight,
           EDefaultShortcutNaviUp,
           EDefaultShortcutNaviDown,
           EDefaultShortcutNaviKey,
           EDefaultShortcutLSK,
           EDefaultShortcutRSK
           };
       
 // Platform default shortcuts.
       const TDefaultShortcut KDefaultShortcuts[] =
       {
       { EDefaultShortcutFirst,     0x00000001, L"localapp:0x101F4CCE" },               // Phonebook,
       { EDefaultShortcutSecond,    0x00000002, L"localapp:0x100058C5" },               // Messaging,
       { EDefaultShortcutThird,     0x00000003, L"localapp:0x10008D39" },               // Browser,
       { EDefaultShortcutFourth,    0x00000004, L"localapp:0x200009EE" },               // Photos,
       { EDefaultShortcutFifth,     0x00000005, L"localapp:0x10005901" },               // Calendar,
       { EDefaultShortcutSixth,     0x00000006, L"localapp:msg?new=msg" },              // New message,
       { EDefaultShortcutSeventh,   0x00000007, L"localapp:0x102072C3" },               // Music Player,
       { EDefaultShortcutEighth,    0x00000008, L"localapp:0x10207A89" },               // Radio,
       { EDefaultShortcutNaviLeft,  0x01000000, L"localapp:0x100058C5?new=msg" },       // New message,
       { EDefaultShortcutNaviRight, 0x01000001, L"localapp:0x10005901" },                // Calendar,
       { EDefaultShortcutNaviUp,    0x01000002, L"localapp:0x101F4CCE" },               // Contacts,
       { EDefaultShortcutNaviDown,  0x01000003, L"localapp:0x101F4CCE" },                // Contacts,
       { EDefaultShortcutNaviKey,   0x01000004, L"localapp:0x100058C5" },               // Messagind,
       { EDefaultShortcutLSK,       0x01000100, L"localapp:0x101F4CD2" },                // Menu,
       { EDefaultShortcutRSK,       0x01000101, L"localapp:0x101F4CCE" }                // Contacts,
       };
       
       const TInt KDefaultShortcutCount = (sizeof(KDefaultShortcuts)/sizeof(KDefaultShortcuts[0])); 
}

inline TPtrC16 DefaultDefinition( const TDefaultShortcut& aShortcut )
    {
    return TPtrC16( (const TText16*) aShortcut.definition );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void GetPlatformDefaultShortcut(TInt aIndex, TUid& aUid, TDes& aDefinition)
    {
    TInt err = KErrNone;
    TAiScutParser parser;
    if (aIndex < 0)
        {
        aIndex = 0;
        }

    aIndex = aIndex % ::KDefaultShortcutCount;
    aDefinition.Copy( ::DefaultDefinition(::KDefaultShortcuts[aIndex]) );

    err = parser.Parse( aDefinition );
    if ( err == KErrNone )
        {
        aUid = TAiScutParser::ParseUid( parser.Get( EScutDefTarget) );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt ResolveCenrepIdFromKey( TInt aKey, TInt &aCenrepId )
    {
    TInt ret = KErrNotFound;
    for ( TInt i = 0; i < KDefaultShortcutCount; ++i )
        {
        if ( KDefaultShortcuts[i].key == aKey )
            {            
            aCenrepId = KDefaultShortcuts[i].id;
            ret = KErrNone;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void TAiDefaultShortcut::GetDefaultShortcut(TInt aKey, TUid& aUid, TDes& aDefinition)
{
    CRepository *cr = NULL;
    TInt err = KErrNone;
    TInt index = KErrNotFound;
    err = ::ResolveCenrepIdFromKey( aKey, index );
    TInt32 crIndex = KAIBackupShortcutDefinitionStart;
    if ( err == KErrNone )
        {
        TRAP( err, cr = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) ) );

        aUid.iUid = KErrNotFound;
        if ( err == KErrNone )
            {
            crIndex += index;
            err = cr->Get( crIndex, aDefinition );
            // In case of a null definition use the platform default
            if ( aDefinition.Length() <= 0 )
                {
                err = KErrNotFound;
                }            
            else if ( err == KErrNone )
                {
                TAiScutParser parser;
                err = parser.Parse( aDefinition );
                if ( err == KErrNone )
                    {
                    aUid = TAiScutParser::ParseUid( parser.Get( EScutDefTarget) );
                    }
                }               
            }           
        }
        
    // Ensure that some definition is returned
    // in case of any error occurrs
    if ( err != KErrNone )
        {
        ::GetPlatformDefaultShortcut( index, aUid, aDefinition );
        }    
    delete cr;
}

// End of File.
