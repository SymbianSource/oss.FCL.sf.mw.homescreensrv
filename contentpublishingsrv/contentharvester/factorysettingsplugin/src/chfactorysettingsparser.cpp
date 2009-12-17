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
* Description:  factory settings for content harvester
 *
*/


// INCLUDE FILES
#include <xcfwtree.h>
#include <xcfwengine.h>
#include <gecoobjectbase.h>
#include <gecodefaultobject.h>
#include <e32const.h>
#include <liwservicehandler.h>

#include "chfactorysettingsparser.h"
#include "chfactorysettingsglobals.h"
#include "chfactsetutils.h"
#include "cpdebug.h"

_LIT8( KType, "type" );
_LIT8( KItem, "item" );
_LIT8( KData, "data" );
_LIT8( KAdd, "Add" );
_LIT8( KDelete, "Delete" );
_LIT( KCpData, "cp_data" );
_LIT( KPublisher, "publisher" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::CCHFactorySettingsParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCHFactorySettingsParser::CCHFactorySettingsParser()
    {
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::ConstructL( MLiwInterface* aInterface )
    {
    iActiveScheduler = new ( ELeave ) CActiveSchedulerWait();

    iContentEngine = CXCFWEngine::NewL( this );
    iCPSInterface = aInterface;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCHFactorySettingsParser* CCHFactorySettingsParser::NewL(
    MLiwInterface* aInterface )
    {
    CCHFactorySettingsParser* self = NewLC( aInterface );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCHFactorySettingsParser* CCHFactorySettingsParser::NewLC(
    MLiwInterface* aInterface )
    {
    CCHFactorySettingsParser* self = new( ELeave ) CCHFactorySettingsParser();
    CleanupStack::PushL( self );
    self->ConstructL( aInterface );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCHFactorySettingsParser::~CCHFactorySettingsParser()
    {
    delete iContentTree;
    delete iContentEngine;
    delete iActiveScheduler;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::RestoreL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCHFactorySettingsParser::RestoreL( const TDesC& aStr, TBool aUnInstall )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::RestoreL" ));
    TInt err;
    iContentTree = CXCFWTree::NewL( );

    iContentEngine->LoadL( *iContentTree, aStr );

    iActiveScheduler->Start( );

    if ( iCompletionStatus == MADatFactorySettingsObserver::ECompleted )
        {
        //this trap ensures stoping the ActiveSheduler
        //in case a leave occurs when updating data
        TRAP( err, HandleEntriesL( aUnInstall ) );
        }

    delete iContentTree;
    iContentTree = NULL;
    return err;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::HandleCompletionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::HandleCompletionL(
    TADatFactorySettingsStatus aStatus )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::HandleCompletionL" ));
    iCompletionStatus = aStatus;
    iActiveScheduler->AsyncStop( );
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::HandleEngineEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::HandleEngineEventL( TXCFWEngineEvent aEvent )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::HandleEngineEventL" ));
    // handle completion
    if ( aEvent == EEvtParsingComplete )
        {
        TADatFactorySettingsStatus status =
                MADatFactorySettingsObserver::ECompleted;
        HandleCompletionL( status );
        }

    // else => ignore
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::HandleEngineErrorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::HandleEngineErrorL( TInt /*aErrorCode*/)
    {
    HandleCompletionL( MADatFactorySettingsObserver::EFailed );
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::HandleEntriesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::HandleEntriesL( TBool aUnInstall )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::HandleEntriesL" ));
    MXCFWNode* root = iContentTree->Root( );
    // get children 
    RNodeArray entries;
    CleanupClosePushL( entries );
    iContentTree->GetChildNodesL( root, entries );
    for ( TInt i = 0; i < entries.Count( ); i++ )
        {
        TPtrC typeIdentifier(entries[i]->Data()->TypeIdentifier( ));

        if ( ( typeIdentifier == KMainDataItem )
            || ( typeIdentifier == KMainPublisherItem ) )
            {
            CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC( );
            CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC( );
            CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
            if ( typeIdentifier == KMainDataItem )
                {
                TLiwGenericParam item_type( KType, TLiwVariant( KCpData ));
                inparam->AppendL( item_type );
                }
            else
                {
                TLiwGenericParam item_type( KType, TLiwVariant( KPublisher ));
                inparam->AppendL( item_type );
                }

            if ( aUnInstall )
                {
                RemoveItemL( entries[i], map );
                TLiwGenericParam item( KData, TLiwVariant( map ));
                inparam->AppendL( item );
                iCPSInterface->ExecuteCmdL( KDelete, *inparam, *outparam );
                }
            else
                {
                AddItemL( entries[i], map, EMap );
                TLiwGenericParam item( KItem, TLiwVariant( map ));
                inparam->AppendL( item );
                iCPSInterface->ExecuteCmdL( KAdd, *inparam, *outparam );
                }
            CleanupStack::PopAndDestroy( map );
            CleanupStack::PopAndDestroy( outparam );
            CleanupStack::PopAndDestroy( inparam );
            }

        }
    // clean up
    CleanupStack::PopAndDestroy( &entries );
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::AddMapL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::AddItemL( MXCFWNode* aItem,
    CLiwContainer* aContainer, TTypes aType )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::AddItemL" ));
    RNodeArray children;
    CleanupClosePushL( children );
    iContentTree->GetChildNodesL( aItem, children );
    for ( TInt i = 0; i < children.Count( ); i++ )
        {
        if ( children[i]->Data()->TypeIdentifier( ) == KDataItem )
            {
            switch ( aType )
                {
                case EMap:
                    ExtractAttributesAndAddL( children[i],
                        static_cast<CLiwDefaultMap*>(aContainer) );
                    break;
                case EList:
                    ExtractAttributesAndAddL( children[i],
                        static_cast<CLiwDefaultList*>(aContainer ) );
                    break;
                default:
                    {
                    User::Panic( _L("Wrong Type" ),1 );
                    }
                }

            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsParser::RemoveItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsParser::RemoveItemL( MXCFWNode* aItem,
    CLiwDefaultMap* aMap )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::RemoveItemL" ));
    RNodeArray children;
    CleanupClosePushL( children );
    iContentTree->GetChildNodesL( aItem, children );
    for ( TInt i = 0; i < children.Count( ); i++ )
        {
        if ( children[i]->Data()->TypeIdentifier( ) == KDataItem )
            {
            ExtractDeleteAttributesL( children[i], aMap );
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CCHFactorySettingsParser::ExtractAttributesAndAddL( MXCFWNode* aItem,
    CLiwDefaultList* aList )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::ExtractAttributesAndAddL" ));
    TPtrC type;
    CGECODefaultObject* node=
            static_cast<CGECODefaultObject*>(aItem->Data( ));
    if ( !node->GetAttribute( KAttributeType, type ) )
        {
        HBufC8* bufV= NULL;
        CLiwDefaultMap* nestedMap= NULL;
        CLiwDefaultList* nestedList= NULL;
        TPtrC value;
        TBool valueExist(node->GetAttribute( KAttributeValue, value ) );
        TTypes typeCheck = ConvertToType( type );
        //check if value exist
        //it is not needed in case of Map and List    
        if ( valueExist==KErrNone || typeCheck==EMap || typeCheck==EList )
            {
            switch ( typeCheck )
                {
                case EInteger:
                    aList->AppendL( FactSetCHUtils::Str2Int32( value ) );
                    break;
                case EUInteger:
                    aList->AppendL( FactSetCHUtils::Str2Uint( value ) );
                    break;
                case EDescriptor8:
                    bufV = FactSetCHUtils::Str2Str8LC( value );
                    aList->AppendL( *bufV );
                    CleanupStack::PopAndDestroy( bufV );
                    break;
                case EDescriptor16:
                    aList->AppendL( value );
                    break;
                case EMap:
                    nestedMap = CLiwDefaultMap::NewLC( );
                    AddItemL( aItem, nestedMap, EMap );
                    aList->AppendL( nestedMap );
                    CleanupStack::PopAndDestroy( nestedMap );
                    break;
                case EList:
                    nestedList = CLiwDefaultList::NewLC( );
                    AddItemL( aItem, nestedList, EList );
                    aList->AppendL( nestedList );
                    CleanupStack::PopAndDestroy( nestedList );
                    break;
                case EBool:
                    aList->AppendL( FactSetCHUtils::Str2Bool( value ) );
                    break;
                case EReal:
                    aList->AppendL( FactSetCHUtils::Str2Real( value ) );
                case EUid:
                    aList->AppendL( FactSetCHUtils::Str2Uid( value ) );
                    break;
                default:
                    {
                    //wrong type - ignore     
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CCHFactorySettingsParser::ExtractAttributesAndAddL( MXCFWNode* aItem,
    CLiwDefaultMap* aMap )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::ExtractAttributesAndAddL" ));
    TPtrC key;
    TPtrC type;
    CGECODefaultObject* node=
            static_cast<CGECODefaultObject*>(aItem->Data( ));
    if ( !node->GetAttribute( KAttributeType, type ) 
        && !node->GetAttribute( KAttributeKey, key ) )
        {
        HBufC8* bufV= NULL;
        CLiwDefaultMap* nestedMap= NULL;
        HBufC8* buf = FactSetCHUtils::Str2Str8LC( key );
        TPtrC value;
        CLiwDefaultList* nestedList= NULL;
        TBool valueExist(node->GetAttribute( KAttributeValue, value ) );
        TTypes typeCheck = ConvertToType( type );
        //check if value exist
        //it is not needed in case of Map and List    
        if ( valueExist==KErrNone || typeCheck==EMap || typeCheck==EList )
            {
            switch ( typeCheck )
                {
                case EInteger:
                    aMap->InsertL( *buf, FactSetCHUtils::Str2Int32( value ) );
                    break;
                case EUInteger:
                    aMap->InsertL( *buf, FactSetCHUtils::Str2Uint( value ) );
                    break;
                case EDescriptor8:
                    bufV = FactSetCHUtils::Str2Str8LC( value );
                    aMap->InsertL( *buf, *bufV );
                    CleanupStack::PopAndDestroy( bufV );
                    break;
                case EDescriptor16:
                    aMap->InsertL( *buf, value );
                    break;
                case EMap:
                    nestedMap = CLiwDefaultMap::NewL( );
                    AddItemL( aItem, nestedMap, EMap );
                    aMap->InsertL( *buf, nestedMap );
                    nestedMap->DecRef( );
                    break;
                case EList:
                    nestedList = CLiwDefaultList::NewL( );
                    AddItemL( aItem, nestedList, EList );
                    aMap->InsertL( *buf, nestedList );
                    nestedList->DecRef( );
                    break;
                case EBool:
                    aMap->InsertL( *buf, FactSetCHUtils::Str2Bool( value ) );
                    break;
                case EReal:
                    aMap->InsertL( *buf, FactSetCHUtils::Str2Real( value ) );
                    break;
                case EUid:
                    aMap->InsertL( *buf, FactSetCHUtils::Str2Uid( value ) );
                    break;
                default:
                    {
                    //wrong type - ignore     
                    }
                }
            }
        CleanupStack::PopAndDestroy( buf );
        }
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
void CCHFactorySettingsParser::ExtractDeleteAttributesL( MXCFWNode* aItem,
    CLiwDefaultMap* aMap )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::ExtractDeleteAttributesL" ));
    TPtrC key;
    TPtrC type;
    CGECODefaultObject* node=
            static_cast<CGECODefaultObject*>(aItem->Data( ));
    if ( !node->GetAttribute( KAttributeType, type ) 
        && !node->GetAttribute( KAttributeKey, key ) )
        {
        HBufC8* bufV= NULL;
        HBufC8* buf = FactSetCHUtils::Str2Str8LC( key );
        TPtrC value;
        TBool valueExist(node->GetAttribute( KAttributeValue, value ) );
        TTypes typeCheck = ConvertToType( type );
        //check if value exist
        if ( valueExist==KErrNone )
            {
            switch ( typeCheck )
                {
                case EDescriptor8:
                    bufV = FactSetCHUtils::Str2Str8LC( value );
                    aMap->InsertL( *buf, *bufV );
                    CleanupStack::PopAndDestroy( bufV );
                    break;
                case EDescriptor16:
                    aMap->InsertL( *buf, value );
                    break;

                default:
                    {
                    //wrong type - ignore     
                    }
                }
            }
        CleanupStack::PopAndDestroy( buf );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//     
CCHFactorySettingsParser::TTypes CCHFactorySettingsParser::ConvertToType(
    const TDesC& aType )
    {
    CP_DEBUG(_L8("CCHFactorySettingsParser::ConvertToType" ));
    if ( !aType.Compare( KInteger ) )
        return EInteger;
    if ( !aType.Compare( KUInteger ) )
        return EUInteger;
    if ( !aType.Compare( KDescriptor8 ) )
        return EDescriptor8;
    if ( !aType.Compare( KDescriptor16 ) )
        return EDescriptor16;
    if ( !aType.Compare( KMap ) )
        return EMap;
    if ( !aType.Compare( KList ) )
        return EList;
    if ( !aType.Compare( KBool ) )
        return EBool;
    if ( !aType.Compare( KReal ) )
        return EReal;
    if ( !aType.Compare( KUid ) )
        return EUid;
    return EUndefined;
    }

//  End of File  
