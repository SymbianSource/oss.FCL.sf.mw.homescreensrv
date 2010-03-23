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

#include <driveinfo.h>
#include <xmlengnodelist.h>
#include <xmlengdomparser.h>
#include <xmlengdocument.h>
#include <utf.h>
#include <liwservicehandler.h>
#include "chfactorysettingsdomparser.h"
#include "chfactorysettingsglobals.h"
#include "chfactsetutils.h"
#include "cpdebug.h"

// CONSTANTS
_LIT8( KMainDataItem8, "data" );
_LIT8( KMainPublisherItem8, "publisher" );
_LIT8( KType, "type" );
_LIT8( KItem, "item" );
_LIT8( KData, "data" );
_LIT8( KAdd, "Add" );
_LIT8( KDelete, "Delete" );
_LIT8( KCpData, "cp_data" );
_LIT8( KPublisher, "publisher" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::CCHFactorySettingsDOMParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCHFactorySettingsDOMParser::CCHFactorySettingsDOMParser( RFs& aFs ):
    iFs( aFs )
    {
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::ConstructL( MLiwInterface* aInterface )
    {
    iCPSInterface = aInterface;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCHFactorySettingsDOMParser* CCHFactorySettingsDOMParser::NewL(
        MLiwInterface* aInterface, RFs& aFs )
    {
    CCHFactorySettingsDOMParser* self = NewLC( aInterface, aFs );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCHFactorySettingsDOMParser* CCHFactorySettingsDOMParser::NewLC(
        MLiwInterface* aInterface, RFs& aFs )
    {
    CCHFactorySettingsDOMParser* self = new( ELeave ) CCHFactorySettingsDOMParser( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aInterface );
    return self;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::~CCHFactorySettingsDOMParser
// Destructor
// -----------------------------------------------------------------------------
//
CCHFactorySettingsDOMParser::~CCHFactorySettingsDOMParser()
    {
    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::Restore
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCHFactorySettingsDOMParser::Restore( const TDesC& aStr, TBool aUnInstall )
    {
    TRAPD( err, ParseFactorySettingsFileL( aStr, aUnInstall ) );
    return err;
    }

// -----------------------------------------------------------------------------
//  CCHFactorySettingsDOMParser::ParseFactorySettingsFileL
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::ParseFactorySettingsFileL(
        const TDesC& aFilePath, TBool aUnInstall )
    {
    RXmlEngDOMImplementation domImpl;
    CleanupClosePushL( domImpl );
    domImpl.OpenL();

    RXmlEngDOMParser parser;
    CleanupClosePushL( parser );
    User::LeaveIfError( parser.Open( domImpl ) );

    RXmlEngDocument doc = parser.ParseFileL( aFilePath );
    CleanupClosePushL( doc );

    TXmlEngElement docElement = doc.AsElement(); // cast node to element node

    TXmlEngElement element = docElement.FirstChild().AsElement();

    RXmlEngNodeList<TXmlEngElement> elementList;
    CleanupClosePushL( elementList );

    element.GetChildElements( elementList );// to remove

    while( elementList.HasNext() )
        {
        element = elementList.Next();
        TPtrC8 typeIdentifier( element.Name() );
        if( !( typeIdentifier.CompareC( KMainDataItem8 ) ) ||
            !( typeIdentifier.CompareC( KMainPublisherItem8 ) ) )
            {
            CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC();
            CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC();
            CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

            if( !( typeIdentifier.CompareC( KMainDataItem8 ) ) )
                {
                TLiwGenericParam item_type( KType, TLiwVariant( KCpData ));
                inparam->AppendL( item_type );
                }
            else
                {
                TLiwGenericParam item_type( KType, TLiwVariant( KPublisher ));
                inparam->AppendL( item_type );
                }

            if( aUnInstall )
                {
                // remove
                RemoveItemL(element, map);
                TLiwGenericParam item( KData, TLiwVariant( map ));
                inparam->AppendL( item );
                iCPSInterface->ExecuteCmdL( KDelete, *inparam, *outparam );
                }
            else
                {
                // add
                AddItemL( element, map, EMap );
                TLiwGenericParam item( KItem, TLiwVariant( map ));
                inparam->AppendL( item );
                iCPSInterface->ExecuteCmdL( KAdd, *inparam, *outparam );
                }

            CleanupStack::PopAndDestroy( map );
            CleanupStack::PopAndDestroy( outparam );
            CleanupStack::PopAndDestroy( inparam );
            }
        }

    CleanupStack::PopAndDestroy( &elementList );
    CleanupStack::PopAndDestroy( &doc );
    CleanupStack::PopAndDestroy( &parser );
    CleanupStack::PopAndDestroy( &domImpl );

    }

// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::AddItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::AddItemL( const TXmlEngElement& aNode,
    CLiwContainer* aContainer, TTypes aType )
    {
    RXmlEngNodeList<TXmlEngElement> children;
    CleanupClosePushL( children );
    aNode.GetChildElements( children );

    while( children.HasNext() )
        {
        TXmlEngElement node = children.Next();
        TPtrC8 typeIdentifier( node.Name() );
        if( !( typeIdentifier.CompareC( KDataItem8 ) ) )
            {
            switch ( aType )
                {
                case EMap:
                    ExtractAttributesAndAddL( node,
                        static_cast<CLiwDefaultMap*>(aContainer) );
                    break;
                case EList:
                    ExtractAttributesAndAddL( node,
                        static_cast<CLiwDefaultList*>( aContainer ) );
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
// CCHFactorySettingsDOMParser::RemoveItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::RemoveItemL( const TXmlEngElement& aNode,
    CLiwDefaultMap* aMap )
    {
    CP_DEBUG(_L8("CCHFactorySettingsDOMParser::RemoveItemL" ));
    RXmlEngNodeList<TXmlEngElement> children;
    CleanupClosePushL( children );

    aNode.GetChildElements( children );

    while( children.HasNext() )
        {
        TXmlEngElement node = children.Next();
        TPtrC8 typeIdentifier( node.Name() );
        if( !( typeIdentifier.CompareC( KDataItem8 ) ) )
            {
            ExtractDeleteAttributesL( node, aMap );
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }


// -----------------------------------------------------------------------------
//  CCHFactorySettingsDOMParser::ExtractDeleteAttributesL
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::ExtractDeleteAttributesL(
		const TXmlEngElement& aNode,
		CLiwDefaultMap* aMap )
    {
    CP_DEBUG(_L8("CCHFactorySettingsDOMParser::ExtractDeleteAttributesL" ));

    TPtrC8 key = aNode.AttributeValueL(KAttributeKey8);
    TPtrC8 value = aNode.AttributeValueL(KAttributeValue8);
    HBufC* type = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aNode.AttributeValueL(KAttributeType8));
    CleanupStack::PushL( type );

    if ( key.Compare( KNullDesC8 ) != 0
        && type->Compare( KNullDesC ) != 0
    	&& value.Compare( KNullDesC8 ) != 0 )
		{
		HBufC* value16 = NULL;
		switch ( ConvertToType( *type ) )
			{
			case EDescriptor8:
				aMap->InsertL( key, value );
				break;
			case EDescriptor16:
			    value16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L( value );
			    CleanupStack::PushL( value16 );
				aMap->InsertL( key, *value16 );
			    CleanupStack::PopAndDestroy( value16 );
				break;
			default:
				{
				//wrong type - ignore
				}
			}
		}
    CleanupStack::PopAndDestroy( type );
    }

// -----------------------------------------------------------------------------
//  CCHFactorySettingsDOMParser::ExtractAttributesAndAddL
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::ExtractAttributesAndAddL(
		const TXmlEngElement& aNode, CLiwDefaultMap* aMap )
    {
    CP_DEBUG(_L8("CCHFactorySettingsDOMParser::ExtractAttributesAndAddL" ));
    TPtrC8 key = aNode.AttributeValueL(KAttributeKey8);
    HBufC* type = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aNode.AttributeValueL(KAttributeType8));
    CleanupStack::PushL( type );

    if ( key.Compare( KNullDesC8 ) != 0
        && type->Compare( KNullDesC ) != 0 )
		{
	    HBufC* value = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
	            aNode.AttributeValueL(KAttributeValue8));
	    CleanupStack::PushL( value );
        TTypes typeCheck = ConvertToType( *type );
        //check if value exist
        //it is not needed in case of Map and List
        if ( value->Compare( KNullDesC ) != 0
        		|| typeCheck == EMap
        		|| typeCheck == EList )
            {
            HBufC8* bufV = NULL;
            CLiwDefaultMap* nestedMap = NULL;
            CLiwDefaultList* nestedList = NULL;

            switch ( typeCheck )
                {
                case EInteger:
                    aMap->InsertL( key, FactSetCHUtils::Str2Int32( *value ) );
                    break;
                case EUInteger:
                	aMap->InsertL( key, FactSetCHUtils::Str2Uint( *value ) );
                    break;
                case EDescriptor8:
                    bufV = FactSetCHUtils::Str2Str8LC( *value );
                    aMap->InsertL( key, *bufV );
                    CleanupStack::PopAndDestroy( bufV );
                    break;
                case EDescriptor16:
                	aMap->InsertL( key, *value );
                    break;
                case EMap:
                	nestedMap = CLiwDefaultMap::NewLC();
                    AddItemL( aNode, nestedMap, EMap );
                    aMap->InsertL( key, nestedMap );
                    CleanupStack::PopAndDestroy( nestedMap );
                    break;
                case EList:
                	nestedList = CLiwDefaultList::NewLC();
                    AddItemL( aNode, nestedList, EList );
                    aMap->InsertL( key, nestedList );
                    CleanupStack::PopAndDestroy( nestedList );
                    break;
                case EBool:
                	aMap->InsertL( key, FactSetCHUtils::Str2Bool( *value ) );
                    break;
                case EReal:
                	aMap->InsertL( key, FactSetCHUtils::Str2Real( *value ) );
                    break;
                case EUid:
                	aMap->InsertL( key, FactSetCHUtils::Str2Uid( *value ) );
                    break;
                default:
                    {
                    //wrong type - ignore
                    }
                }
            }
        CleanupStack::PopAndDestroy( value );
		}
    CleanupStack::PopAndDestroy( type );
    }


// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::ExtractAttributesAndAddL
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsDOMParser::ExtractAttributesAndAddL(
        const TXmlEngElement& aNode, CLiwDefaultList* aList )
    {
    CP_DEBUG( _L8( "CCHFactorySettingsDOMParser::ExtractAttributesAndAddL" ) );

    HBufC* type = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aNode.AttributeValueL(KAttributeType8));
    CleanupStack::PushL( type );

    if( type->Compare( KNullDesC ) != 0 )
        {
        HBufC* value = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
                aNode.AttributeValueL(KAttributeValue8));
        CleanupStack::PushL( value );
        TTypes typeCheck = ConvertToType( *type );
        //check if value exist
        //it is not needed in case of Map and List
        if( value->Compare( KNullDesC ) != 0
                || typeCheck == EMap
                || typeCheck == EList )
            {
            HBufC8* bufV = NULL;
            CLiwDefaultMap* nestedMap = NULL;
            CLiwDefaultList* nestedList = NULL;

            switch( typeCheck )
                {
                case EInteger:
                    aList->AppendL( FactSetCHUtils::Str2Int32( *value ) );
                    break;
                case EUInteger:
                    aList->AppendL( FactSetCHUtils::Str2Uint( *value ) );
                    break;
                case EDescriptor8:
                    bufV = FactSetCHUtils::Str2Str8LC( *value );
                    aList->AppendL( *bufV );
                    CleanupStack::PopAndDestroy( bufV );
                    break;
                case EDescriptor16:
                    aList->AppendL( value );
                    break;
                case EMap:
                    nestedMap = CLiwDefaultMap::NewLC();
                    AddItemL( aNode, nestedMap, EMap );
                    aList->AppendL( nestedMap );
                    CleanupStack::PopAndDestroy( nestedMap );
                    break;
                case EList:
                    nestedList = CLiwDefaultList::NewLC();
                    AddItemL( aNode, nestedList, EList );
                    aList->AppendL( nestedList );
                    CleanupStack::PopAndDestroy( nestedList );
                    break;
                case EBool:
                    aList->AppendL( FactSetCHUtils::Str2Bool( *value ) );
                    break;
                case EReal:
                    aList->AppendL( FactSetCHUtils::Str2Real( *value ) );
                    break;
                case EUid:
                    aList->AppendL( FactSetCHUtils::Str2Uid( *value ) );
                    break;
                default:
                    {
                    //wrong type - ignore
                    }
                }
            }
        CleanupStack::PopAndDestroy( value );
        }
    CleanupStack::PopAndDestroy( type );
    }


// -----------------------------------------------------------------------------
// CCHFactorySettingsDOMParser::ConvertToType
// -----------------------------------------------------------------------------
//
CCHFactorySettingsDOMParser::TTypes CCHFactorySettingsDOMParser::ConvertToType(
    const TDesC& aType )
    {
    CP_DEBUG(_L8("CCHFactorySettingsDOMParser::ConvertToType" ));
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
