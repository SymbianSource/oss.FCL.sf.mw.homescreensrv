/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class keeps BS configuration
 *
*/


#include <bautils.h> //FileExists
#include <xcfwtree.h>
#include <xcfwengine.h>
#include <gecoobjectbase.h>
#include <gecodefaultobject.h>

#include "bsdebug.h"
#include "bsconfiguration.h"

_LIT(KConfigurationFile, "bs_config.xml" );
_LIT(KCDrive, "C");
_LIT(KZDrive, "Z");

_LIT(KDelimiter, ":");
_LIT(KTrueValue, "1");
_LIT(KHexStart,"0x");

_LIT(KAppNode, "app");

_LIT(KUidAttribute, "uid");
_LIT(KResetAttribute, "reset");
_LIT(KIgnoredAttribute, "ignored");
_LIT(KIgnoredIfStartedFromAttribute, "ignoredIfStartedFrom");
_LIT(KResetIfThruAttribute, "resetIfThru");

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CBSConfiguration::CBSConfiguration()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CBSConfiguration::~CBSConfiguration()
    {
    delete iContentTree;
    delete iContentEngine;

    iResetApps.Close( );
    iIgnoredApps.Close( );
    iIgnoredIfStartedFrom.Close( );
    iResetIfThru.Close( );
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSConfiguration* CBSConfiguration::NewLC()
    {
    CBSConfiguration* self = new (ELeave)CBSConfiguration();
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSConfiguration* CBSConfiguration::NewL()
    {
    CBSConfiguration* self = CBSConfiguration::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBSConfiguration::ConstructL()
    {
    iContentTree = CXCFWTree::NewL( );
    iContentEngine = CXCFWEngine::NewL( this );

    RFs fileSession;

    User::LeaveIfError( fileSession.Connect( ) );
    CleanupClosePushL( fileSession );

    TFileName configPath;

    User::LeaveIfError( fileSession.PrivatePath( configPath ) );
    configPath.Insert( 0, KDelimiter );
    configPath.Insert( 0, KCDrive );
    configPath.Append( KConfigurationFile );

    // first check if file exists on c drive 
    if ( !BaflUtils::FileExists( fileSession, configPath ) )
        {
        // read from z drive 
        configPath[0] = KZDrive()[0];
        }

    CleanupStack::PopAndDestroy( &fileSession );

    iContentEngine->LoadL( *iContentTree, configPath );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
TBool CBSConfiguration::IsReset( TUid aApp )
    {
    return iResetApps.Find( aApp.iUid ) != KErrNotFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//        
TBool CBSConfiguration::IsIgnored( TUid aApp )
    {
    return iIgnoredApps.Find( aApp.iUid ) != KErrNotFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//        
TBool CBSConfiguration::IsIgnoredIfStartedFrom( TUid aApp )
    {
    return iIgnoredIfStartedFrom.Find( aApp.iUid ) != KErrNotFound;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//        
RArray<TInt>& CBSConfiguration::ResetIfThruList()
    {
    return iResetIfThru;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSConfiguration::LoadConfigurationL()
    {
    // get root node
    MXCFWNode* root = iContentTree->Root( );

    // list of fetched nodes
    RNodeArray nodes;
    CleanupClosePushL( nodes );

    // handle entries
    iContentTree->GetNodesOfTypeL( KAppNode, nodes, root, ETrue );

    for ( TInt i = 0; i < nodes.Count( ); i++ )
        {
        CGECODefaultObject* object =
                static_cast<CGECODefaultObject*>(nodes[i]->Data( ));
        TPtrC uidVal;
        TPtrC resetVal;
        TPtrC ignoredVal;
        TPtrC ignoredIfStartedFromVal;
        TPtrC resetIfThruVal;
        object->GetAttribute( KUidAttribute, uidVal );
        object->GetAttribute( KResetAttribute, resetVal );
        object->GetAttribute( KIgnoredAttribute, ignoredVal );
        object->GetAttribute( KIgnoredIfStartedFromAttribute,
            ignoredIfStartedFromVal );
        object->GetAttribute( KResetIfThruAttribute, resetIfThruVal );

        TInt uid = Str2Int( uidVal );

        if ( resetVal.Compare( KTrueValue ) == 0 )
            iResetApps.AppendL( uid );

        if ( ignoredVal.Compare( KTrueValue ) == 0 )
            iIgnoredApps.AppendL( uid );

        if ( ignoredIfStartedFromVal.Compare( KTrueValue ) == 0 )
            iIgnoredIfStartedFrom.AppendL( uid );

        if ( resetIfThruVal.Compare( KTrueValue ) == 0 )
            iResetIfThru.AppendL( uid );
        }

#ifdef _DEBUG    

    for ( TInt i = 0; i< iResetApps.Count( ); i++ )
        DEBUG(("\tiResetApps[%d] = %X)", i, iResetApps[i]));

    for ( TInt i = 0; i< iIgnoredApps.Count( ); i++ )
        DEBUG(("\tiIgnoredApps[%d] = %X)", i, iIgnoredApps[i]));

    for ( TInt i = 0; i< iIgnoredIfStartedFrom.Count( ); i++ )
        DEBUG(("\tiIgnoredIfStartedFrom[%d] = %X)", i, iIgnoredIfStartedFrom[i]));

    for ( TInt i = 0; i< iResetIfThru.Count( ); i++ )
        DEBUG(("\tiResetIfThru[%d] = %X)", i, iResetIfThru[i]));

#endif

    // clean up
    CleanupStack::PopAndDestroy( &nodes );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//   
TInt CBSConfiguration::Str2Int( const TDesC& aStr )
    {
    // if a string starts from '0x' then convert aStr from hex to int.
    // else send aStr to the StrDec2Uint

    TInt position(aStr.Find( KHexStart ) );
    TInt ret(KErrArgument);
    if ( position == 0 )
        {
        // is hex
        TPtrC string(aStr.Mid( KHexStart().Length( ) ) );

        TLex lexer(string);
        TUint val;
        lexer.Val( val, EHex );
        ret = val;
        }
    else
        {
        TLex lexer(aStr);
        lexer.Val( ret );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// From class MXCFWEngineObserver
// 
// -----------------------------------------------------------------------------
//
void CBSConfiguration::HandleEngineEventL( TXCFWEngineEvent aEvent )
    {
    // handle completion
    if ( aEvent == EEvtParsingComplete )
        {
        LoadConfigurationL( );
        }
    }

// -----------------------------------------------------------------------------
// From class MXCFWEngineObserver
// 
// -----------------------------------------------------------------------------
//
void CBSConfiguration::HandleEngineErrorL( TInt /*aErrorCode*/)
    {
    }
