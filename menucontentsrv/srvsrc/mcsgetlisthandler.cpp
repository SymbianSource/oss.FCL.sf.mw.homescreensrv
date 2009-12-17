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
* Description:  
*
*/

#include <e32cmn.h>
#include <liwcommon.h>
#include <liwgenericparam.h>
#include <s32mem.h>

#include "mcsdef.h"
#include "mcsgetlisthandler.h"
#include "mcsmenufilter.h"
#include "menusrveng.h"
#include "menusrvobjectfilter.h"
#include "mcsgetlistcreatorinterface.h"
#include "mcsgetlistflatcreator.h"
#include "mcsgetlisttreecreator.h"

// ---------------------------------------------------------
// CMcsGetlistHandler::NewL
// ---------------------------------------------------------
//
CMcsGetlistHandler* CMcsGetlistHandler::NewL( CMenuSrvEng& aEng )
	{
	CMcsGetlistHandler* self;
	self = new ( ELeave ) CMcsGetlistHandler( aEng );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsGetlistHandler::~CMcsGetlistHandler
// ---------------------------------------------------------
//
CMcsGetlistHandler::~CMcsGetlistHandler()
    {
    CloseOutputBuffer();
    }

// ---------------------------------------------------------
// CMcsGetlistHandler::~CMcsGetlistHandler
// ---------------------------------------------------------
//
void CMcsGetlistHandler::CloseOutputBuffer()
	 {
	 iOutputBuffer.Close();
	 }

// ---------------------------------------------------------
// CMcsGetlistHandler::CMcsGetlistHandler
// ---------------------------------------------------------
//
CMcsGetlistHandler::CMcsGetlistHandler( CMenuSrvEng& aEng ):
	iEng(aEng)
    {
    }

// ---------------------------------------------------------
// CMcsGetlistHandler::ConstructL
// ---------------------------------------------------------
//
void CMcsGetlistHandler::ConstructL()
    {

    }

// ---------------------------------------------------------
// CMcsGetlistHandler::GetListSizeL
// ---------------------------------------------------------
//
TInt CMcsGetlistHandler::GetListSizeL( const TDesC8& aSerializedInput )
    {
    CloseOutputBuffer();
	//Internalize
    CLiwGenericParamList* inputList = CLiwGenericParamList::NewLC();
	RDesReadStream stream( aSerializedInput );
	CleanupClosePushL( stream );
	inputList->InternalizeL( stream ) ;
	CleanupStack::PopAndDestroy( &stream );

	//fetch basic parameters
	TInt folderId( KErrNotFound );
	TBasicParameters basicParameters; //flag - TBasicParameters
	FetchBasicParametersL( inputList, folderId, basicParameters );

	//fetch required and ignored attributes
    CDesC16Array* ignoredAttributes = new (ELeave) CDesC16ArrayFlat(1);
    CleanupStack::PushL( ignoredAttributes );
    CDesC16Array* requiredAttributes = new (ELeave) CDesC16ArrayFlat(8);
    CleanupStack::PushL( requiredAttributes );
	FetchRequiredAndIgnoredAttributesL( inputList,
				requiredAttributes, ignoredAttributes );

	//fetch filter
	CMenuFilter* filter = CMenuFilter::NewLC();
	if ( !( basicParameters & EGetListParentOnly ) )
		{
		if ( FetchFilterL( inputList, filter ) )
			{
			//if filter is present we change result type to "flat"
			basicParameters |= EGetListFlat;
			}
		}
	RArray<TMenuItem> itemsArray;
	CleanupClosePushL ( itemsArray );
	FetchItemsIdsL( itemsArray, folderId, filter, basicParameters );

	//create output list
	CMcsGetListCreatorInterface* outputListCreator;
	if ( basicParameters & EGetListFlat )
		{
		outputListCreator = CMcsGetlistFlatCreator::NewL( iEng );
		}
	else
		{
		outputListCreator = CMcsGetlistTreeCreator::NewL( iEng );
		}
	CleanupStack::PushL( outputListCreator );
	CLiwDefaultList* outputList =
		outputListCreator->CreateLC( itemsArray, requiredAttributes,
				ignoredAttributes );

	TLiwVariant lvList( outputList );
	lvList.PushL();
	TLiwGenericParam lgpList( KReturnValue, lvList );
	lgpList.PushL();

	CLiwGenericParamList* outParamList = CLiwGenericParamList::NewLC();
	outParamList->AppendL( lgpList );
	iOutputBuffer.CreateL( outParamList->Size() );

	RDesWriteStream datastrm( iOutputBuffer );
	CleanupClosePushL(datastrm);
	outParamList->ExternalizeL(datastrm);
	datastrm.CommitL();

    CleanupStack::PopAndDestroy(&datastrm);
	CleanupStack::PopAndDestroy( outParamList );
	CleanupStack::PopAndDestroy( &lgpList );
	CleanupStack::PopAndDestroy( &lvList );
	CleanupStack::PopAndDestroy( outputList );
	CleanupStack::PopAndDestroy( outputListCreator );
	CleanupStack::PopAndDestroy( &itemsArray );
	CleanupStack::PopAndDestroy( filter );
	CleanupStack::PopAndDestroy( requiredAttributes );
	CleanupStack::PopAndDestroy( ignoredAttributes );
	CleanupStack::PopAndDestroy( inputList );
    return iOutputBuffer.Size();
    }

// ---------------------------------------------------------
// CMcsGetlistHandler::GetListSizeL
// ---------------------------------------------------------
//
TPtrC8 CMcsGetlistHandler::GetListDataL( )
    {
    return TPtrC8( iOutputBuffer );
    }

// ---------------------------------------------------------
// CMcsGetlistHandler::FetchBasicParametersL
// ---------------------------------------------------------
//
void CMcsGetlistHandler::FetchBasicParametersL(
		const CLiwGenericParamList* aInputList,
		TInt& aFolderId, TBasicParameters& aBasicParameters )
	{
	TInt error( KErrNone );
	TInt pos = 0;
	aBasicParameters = !EGetListRecursive |
						EGetListFlat |
					   !EGetListParentOnly |
					   !EGetListIncludeParent;

	const TLiwGenericParam* inParam = aInputList->FindFirst( pos, KInData );
	if (inParam)
	    {
	    const CLiwMap* inputMap = inParam->Value().AsMap();
	    if ( inputMap )
            {
            TLiwVariant param;
            // Folder Id
            if( inputMap->FindL( KMenuAttrId8, param ) )
                {
                TInt32 folderId( KErrNotFound );
                param.Get( folderId );
                aFolderId = folderId;
                }
            else
            	{
            	error = KErrArgument;
            	}
            param.Reset();
        	if ( aFolderId == 0 )
        		{
        		iEng.Engine().RootFolderL( aFolderId );
        		}

        	// Include parent in the result
        	if( inputMap->FindL( KIncludeParent, param) )
        		{
        		if ( param.AsTBool() )
        			{
        			aBasicParameters |= EGetListIncludeParent;
        			}
        		}
        	param.Reset();

            // GetList search type
            if( inputMap->FindL( KRecursiveSearch, param ) )
                {
                if ( param.AsTBool( ) )
                	{
                	aBasicParameters |= EGetListRecursive;
                	}
	            }
	        param.Reset();

	        // Result List type (Flat/tree)
	        if( inputMap->FindL( KFlatResult, param ) )
                {
                if ( !param.AsTBool() )
                   	{
                   	aBasicParameters &= ~EGetListFlat;
                   	}
                }
            param.Reset();

            // Search only specific Item
            if( inputMap->FindL( KParentOnly, param ) )
                {
                if ( param.AsTBool() )
                   	{
                   	aBasicParameters |= EGetListParentOnly;
                   	aBasicParameters |= EGetListFlat;
                   	}
                }
            param.Reset();
            }
	    else
	    	{
	    	error = KErrArgument;
	    	}
	    }
	else
		{
    	error = KErrArgument;
		}
	User::LeaveIfError( error );
	}

// ---------------------------------------------------------
// CMcsGetlistHandler::FetchRequiredAndIgnoredAttributesL
// ---------------------------------------------------------
//
void CMcsGetlistHandler::FetchRequiredAndIgnoredAttributesL(
		const CLiwGenericParamList* aInputList,
		CDesC16Array* aRequiredAttributes,
		CDesC16Array* aIgnoredAttributes )
	{
	TInt pos = 0;
	const TLiwGenericParam* inFilter = aInputList->FindFirst( pos, KFilter );
	if (inFilter)
        {
        const CLiwMap* inputMap = inFilter->Value().AsMap();
        if( inputMap )
            {
            TLiwVariant variant;
            variant.PushL();
            if( inputMap->FindL( KIgnoreAttributes, variant ) )
                {
                GetAttributesL( variant.AsDes(), aIgnoredAttributes );
                }
            if( inputMap->FindL( KRequiredAttributes, variant ) )
                {
                GetAttributesL( variant.AsDes(), aRequiredAttributes );
                }
            CleanupStack::PopAndDestroy( &variant );
            }
        }
	}

// ---------------------------------------------------------
// CMcsGetlistHandler::FetchFilterL
// ---------------------------------------------------------
//
TBool CMcsGetlistHandler::FetchFilterL( const CLiwGenericParamList* aInputList,
		CMenuFilter* aFilter )
	{
	TBool filterPresent( EFalse );
	TInt pos = 0;
	const TLiwGenericParam* inFilter = aInputList->FindFirst( pos, KFilter );
	if ( inFilter )
        {
        const CLiwMap* inputMap = inFilter->Value().AsMap();
        if ( inputMap )
            {
            TLiwVariant param;
            filterPresent = ETrue;

           	// Type
            if( inputMap->FindL( KType, param) )
                {
                aFilter->SetType( param.AsDes( ) );
                }
            param.Reset();
           	SetFlagsL( inputMap, aFilter );
           	// ref_id
            SetAttributeNumL( inputMap, KMenuAttrRefId( ), aFilter );
            SetAttributeDesL( inputMap, KMenuAttrShortName( ), aFilter );
            SetAttributeDesL( inputMap, KMenuAttrTitleName( ), aFilter );
            SetAttributeDesL( inputMap, KMenuAttrLongName( ), aFilter );
            SetAttributeDesL( inputMap, KMenuAttrIconFile( ), aFilter );
            SetAttributeDesL( inputMap, KMenuAttrAppGroupName( ), aFilter );
            SetAttributeHexFormatL( inputMap, KMenuAttrUid( ), aFilter );
            SetAttributeBoolL( inputMap, KMenuAttrDefault( ), aFilter );
            SetAttributeHexFormatL( inputMap, KMenuAttrView( ), aFilter );
            SetAttributeDesL( inputMap, KMenuAttrUrl( ), aFilter );
            SetAttributeHexFormatL( inputMap, KMenuAttrIconId( ), aFilter );
            SetAttributeHexFormatL( inputMap, KMenuAttrMaskId( ), aFilter );
            SetAttributeHexFormatL( inputMap, KMenuAttrIconSkinMajorId( ),
            		aFilter );
            SetAttributeHexFormatL( inputMap, KMenuAttrIconSkinMinorId( ),
            		aFilter );
            SetAttributeDesL( inputMap, KMenuAttrM3gIconFile( ), aFilter );
            }
        }
	return filterPresent;
	}

// ---------------------------------------------------------------------------
// CMcsGetlistHandler::FetchItemsIdsL
// ---------------------------------------------------------------------------
void CMcsGetlistHandler::FetchItemsIdsL( RArray<TMenuItem>& aItemArray,
		TInt aFolderId, CMenuFilter* aFilter,
		TBasicParameters& aBasicParameters )
    {
    if( aBasicParameters & ( EGetListParentOnly | EGetListIncludeParent ) )
    	{
        TMenuItem item;
        iEng.Engine().GetItemL( aFolderId, item );
        aItemArray.AppendL( item );
    	}
    if( !( aBasicParameters & EGetListParentOnly ) )
        {
        TMenuSrvObjectFilter engFilter( *aFilter );
        iEng.Engine().GetItemsL( aItemArray, aFolderId, &engFilter,
        		aBasicParameters & EGetListRecursive );
        }
    }


// ---------------------------------------------------------------------------
// CMcsGetlistHandler::GetAttributesL
// ---------------------------------------------------------------------------
//
void CMcsGetlistHandler::GetAttributesL( const TDesC& aList, CDesC16Array* aArray )
    {
    TLex input( aList );
    TLexMark startMark;
    input.Mark( startMark );
    TBool notEmpty = EFalse;
    while ( !input.Eos() )
        {
        if( input.Peek() == ':')
            {
            aArray->AppendL( input.MarkedToken( startMark ) );
            input.Inc();
            input.Mark( startMark );
            }
        input.Inc();
        notEmpty = ETrue;
        }
    if ( notEmpty )
        {
        aArray->AppendL( input.MarkedToken( startMark ) );
        }
    }

// ---------------------------------------------------------------------------
// CMcsGetlistHandler::SetFlagsL
// ---------------------------------------------------------------------------
//
void CMcsGetlistHandler::SetFlagsL( const CLiwMap* aInputMap,
		CMenuFilter* aFilter )
    {
    TInt flagson = 0;
    TInt flagsoff = 0;
    TLiwVariant param;
    param.PushL( );
    if( aInputMap->FindL( KNameLocked, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::ELockName;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::ELockName;
             }
        }
    param.Reset();

    if( aInputMap->FindL( KIconLocked, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::ELockIcon;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::ELockIcon;
             }
        }
    param.Reset();

    if( aInputMap->FindL( KDeleteLocked, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::ELockDelete;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::ELockDelete;
             }
        }
    param.Reset();

    if( aInputMap->FindL( KMoveLocked, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::ELockItem;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::ELockItem;
             }
        }
    param.Reset();

    if( aInputMap->FindL( KAddingLocked, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::ELockMoveInto;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::ELockMoveInto;
             }
        }
    param.Reset();

    if( aInputMap->FindL( KHidden, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::EHidden;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::EHidden;
             }
        }
    param.Reset();

    if( aInputMap->FindL( KMissing, param))
        {
         if(param.AsTBool())
             {
             flagson = flagson + TMenuItem::EMissing;
             }
         else
             {
             flagsoff = flagsoff + TMenuItem::EMissing;
             }
        }
    aFilter->SetFlags(flagson,flagsoff);
    CleanupStack::PopAndDestroy( &param );
    }

// ---------------------------------------------------------------------------
// CMcsGetlistHandler::SetAttributeDesL
// ---------------------------------------------------------------------------
//
void CMcsGetlistHandler::SetAttributeDesL( const CLiwMap* aInputMap,
                               const TDesC& aAttrName, CMenuFilter* aFilter )
    {
    TLiwVariant param;
    param.PushL( );
    TBuf8<KMenuMaxAttrNameLen> attrname;
    attrname.Copy( aAttrName );
    if( aInputMap->FindL( attrname, param ) )
        {
        aFilter->HaveAttributeL(aAttrName, param.AsDes() );
        }
    CleanupStack::PopAndDestroy( &param );
    }

// ---------------------------------------------------------------------------
// CMcsGetlistHandler::SetAttributeHexFormatL
// ---------------------------------------------------------------------------
//
void CMcsGetlistHandler::SetAttributeHexFormatL( const CLiwMap* aInputMap,
                                const TDesC& aAttrName, CMenuFilter* aFilter )
    {
    TLiwVariant param;
    param.PushL( );
    TBuf8<KMenuMaxAttrNameLen> attrname;
    TBuf16<KMenuMaxAttrValueLen> paramValStr;
    attrname.Copy( aAttrName );
    if( aInputMap->FindL( attrname, param ) )
        {
        TUint temp;
        param.Get(temp);
        paramValStr.Format( KHexFormat, temp );
        aFilter->HaveAttributeL( aAttrName, paramValStr );
        }
    CleanupStack::PopAndDestroy( &param );
    }

// ---------------------------------------------------------------------------
// CMcsGetlistHandler::SetAttributeBoolL
// ---------------------------------------------------------------------------
//
void CMcsGetlistHandler::SetAttributeBoolL( const CLiwMap* aInputMap,
                               const TDesC& aAttrName, CMenuFilter* aFilter )
    {
    TLiwVariant param;
    param.PushL( );
    TBuf8<KMenuMaxAttrNameLen> attrname;
    TBuf16<KMenuMaxAttrValueLen> paramValStr;
    attrname.Copy( aAttrName );
    if( aInputMap->FindL( attrname, param ) )
        {
        paramValStr.Num( param.AsTBool());
        aFilter->HaveAttributeL(aAttrName, paramValStr );
        }
    CleanupStack::PopAndDestroy( &param );
    }

// ---------------------------------------------------------------------------
// CMcsGetlistHandler::SetAttributeNumL
// ---------------------------------------------------------------------------
//
void CMcsGetlistHandler::SetAttributeNumL( const CLiwMap* aInputMap,
                             const TDesC& aAttrName, CMenuFilter* aFilter )
    {
    TLiwVariant param;
    param.PushL( );
    TBuf8<KMenuMaxAttrNameLen> attrname;
    TBuf16<KMenuMaxAttrValueLen> paramValStr;
    attrname.Copy( aAttrName );
    if( aInputMap->FindL( attrname, param ) )
        {
        TUint temp;
        param.Get(temp);
        paramValStr.Num( temp );
        aFilter->HaveAttributeL( aAttrName, paramValStr );
        }
    CleanupStack::PopAndDestroy( &param );
    }



//  End of File
