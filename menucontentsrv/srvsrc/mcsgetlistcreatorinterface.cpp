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
#include "mcsgetlistcreatorinterface.h"
#include "menusrveng.h"

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::~CMcsGetListCreatorInterface
// ---------------------------------------------------------
//
CMcsGetListCreatorInterface::~CMcsGetListCreatorInterface()
    {
    }

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::CMcsGetListCreatorInterface
// ---------------------------------------------------------
//
CMcsGetListCreatorInterface::CMcsGetListCreatorInterface( CMenuSrvEng& aEng ):
	iEng( aEng )
    {
    }

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::ConstructL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::ConstructL()
    {
    }

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::BuildMapL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::BuildMapL(
		const TMenuItem& aMenuItem, CDesC16Array* aRequiredAttributes,
		CDesC16Array* aIgnoredAttributes, CLiwDefaultMap* aMap )
	{
	SetItemIdL( aMenuItem.Id() );
	AddFundamentalAttributesL( aMenuItem, aMap );

	if( aRequiredAttributes->MdcaCount() &&
			aIgnoredAttributes->MdcaCount() == KErrNone )
	    {
	    AddRequiredAttributesL( aMenuItem, aRequiredAttributes, aMap );
	    }
	else if( aIgnoredAttributes->MdcaCount() &&
			aRequiredAttributes->MdcaCount() == KErrNone )
	    {
	    AddAttributesWithoutIgnoredL( aMenuItem, aIgnoredAttributes, aMap );
	    }
	else if( aRequiredAttributes->MdcaCount() )
	    {
	    AddRequiredAttributesWithoutIgnoredL( aMenuItem, aRequiredAttributes,
	    		aIgnoredAttributes, aMap );
	    }
	else
	    {
	    AddAllAttributesL( aMenuItem, aMap );
	    }
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::AddFundamentalAttributesL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::AddFundamentalAttributesL(
		const TMenuItem& aMenuItem, CLiwDefaultMap* aMap )
	{
    aMap->InsertL( KMenuAttrId8, TLiwVariant( (TInt32)aMenuItem.Id() ) );

    aMap->InsertL( KType, TLiwVariant( aMenuItem.Type() ) );
    aMap->InsertL( KMenuParentId, TLiwVariant( (TInt32)aMenuItem.Parent() ) );

    // Flags
    TInt flag = aMenuItem.Flags();
    TInt flagvalue = ( flag & TMenuItem::ELockDelete )? 1 : 0;

    aMap->InsertL( KDeleteLocked, TLiwVariant( (TBool)flagvalue) );

    flagvalue = ( flag & TMenuItem::ELockName )? 1 : 0;
    aMap->InsertL( KNameLocked, TLiwVariant( (TBool)flagvalue) );

    flagvalue = ( flag & TMenuItem::ELockIcon )? 1 : 0;
    aMap->InsertL( KIconLocked, TLiwVariant( (TBool)flagvalue) );

    flagvalue = ( flag & TMenuItem::ELockMoveInto )? 1 : 0;
    aMap->InsertL( KAddingLocked, TLiwVariant( (TBool)flagvalue) );

    flagvalue = ( flag & TMenuItem::ELockItem )? 1 : 0;
    aMap->InsertL( KMoveLocked, TLiwVariant( (TBool)flagvalue) );

    flagvalue = ( flag & TMenuItem::EHidden )? 1 : 0;
    aMap->InsertL( KHidden, TLiwVariant( (TBool)flagvalue) );

    flagvalue = ( flag & TMenuItem::EMissing )? 1 : 0;
    aMap->InsertL( KMissing, TLiwVariant( (TBool)flagvalue) );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::AddRequiredAttributesL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::AddRequiredAttributesL( const TMenuItem& aMenuItem,
		CDesC16Array* aRequiredAttributes, CLiwDefaultMap* aMap )
	{
	TBool exists = EFalse;
	TBuf<KMenuMaxAttrNameLen> running;
	running.Copy( KRunning );
	TBuf<KMenuMaxAttrNameLen> bitmap;
	bitmap.Copy( KBitmapBuffer );
	TBuf<KMenuMaxAttrNameLen> mask;
	mask.Copy( KMaskBuffer );
	TBool findBitmap( EFalse );
	TBool findMask( EFalse );

	RBuf attrvalue;
	attrvalue.CreateL( KMenuMaxAttrValueLen );
	CleanupClosePushL( attrvalue );

	for( TInt i= 0; i < aRequiredAttributes->MdcaCount(); i++ )
	    {
	    if( aRequiredAttributes->MdcaPoint(i).Compare( running ) == KErrNone )
	        {
	        if ( aMenuItem.Type() == KMenuTypeApp() ||
	        		aMenuItem.Type() == KMenuTypeFolder() )
	            {
	            iEng.GetAttributeL( iId, KRunningStatus, exists, attrvalue );
	            aMap->InsertL( KRunning, TLiwVariant( exists ) );
	            }
	        }
	    else if( aRequiredAttributes->MdcaPoint(i).Compare( bitmap ) == KErrNone )
	        {
	        findBitmap = ETrue;
	        }
	    else if( aRequiredAttributes->MdcaPoint(i).Compare( mask ) == KErrNone  )
	        {
	        findMask = ETrue;
	        }
	    else
	        {
            iEng.GetAttributeL(
                    iId, aRequiredAttributes->MdcaPoint(i), exists, attrvalue );

	        if( exists )
	            {
	            TBuf8<KMenuMaxAttrNameLen> attrname;
	            attrname.Copy( aRequiredAttributes->MdcaPoint(i) );
	            aMap->InsertL( attrname, TLiwVariant( attrvalue ) );
	            }
	        }
	    }
	if( findBitmap && findMask )
	    {
	    CLiwDefaultMap* iconAttrMap = CLiwDefaultMap::NewLC();
	    FillIconAttributesMapL( aMenuItem, iconAttrMap );
		aMap->InsertL( KIconAttributes, TLiwVariant( iconAttrMap ) );
    	CleanupStack::PopAndDestroy( iconAttrMap );
	   	}
	CleanupStack::PopAndDestroy( &attrvalue );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::AddAttributesWithoutIgnoredL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::AddAttributesWithoutIgnoredL(	const TMenuItem& aMenuItem,
		CDesC16Array* aIgnoredAttributes, CLiwDefaultMap* aMap )
	{
    TBool exists = EFalse;
    TInt pos( KErrNotFound );

	RBuf attrvalue;
	attrvalue.CreateL( KMenuMaxAttrValueLen );
	CleanupClosePushL( attrvalue );

    if ( aMenuItem.Type() == KMenuTypeApp()
    		|| aMenuItem.Type() == KMenuTypeFolder() )
        {
        TBuf<KMenuMaxAttrNameLen> running;
        running.Copy( KRunning );
        if( aIgnoredAttributes->Find( running, pos ) != KErrNone )
            {
            iEng.GetAttributeL( iId, KRunningStatus, exists, attrvalue );
            aMap->InsertL( KRunning, TLiwVariant( exists ) );
            }
        }

    RArray<TPtrC> attrNameList;
    CleanupClosePushL(attrNameList);
    iEng.GetAttributeListL( aMenuItem.Id(), attrNameList );

    for( TInt i = 0; i< attrNameList.Count(); i++ )
        {
        if( aIgnoredAttributes->Find( attrNameList[i], pos ) != KErrNone )
            {
            iEng.GetAttributeL( iId, attrNameList[i], exists, attrvalue );
            if( exists )
                {
                TBuf8<KMenuMaxAttrNameLen> attrname;
                attrname.Copy( attrNameList[i] );
                aMap->InsertL( attrname, TLiwVariant( attrvalue ) );
                }
            }
        }
    TBuf<KMenuMaxAttrNameLen> bitmap;
    bitmap.Copy( KBitmapBuffer );
    TBuf<KMenuMaxAttrNameLen> mask;
    mask.Copy( KMaskBuffer );
    if( aIgnoredAttributes->Find( bitmap, pos ) != KErrNone
        && aIgnoredAttributes->Find( mask, pos ) != KErrNone )
        {
        CLiwDefaultMap* iconAttrMap = CLiwDefaultMap::NewLC();
    	FillIconAttributesMapL( aMenuItem, iconAttrMap );
    	aMap->InsertL( KIconAttributes, TLiwVariant( iconAttrMap ) );
    	CleanupStack::PopAndDestroy( iconAttrMap );
    	}

    CleanupStack::PopAndDestroy(&attrNameList);

	CleanupStack::PopAndDestroy( &attrvalue );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::AddRequiredAttributesWithoutIgnoredL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::AddRequiredAttributesWithoutIgnoredL(
		const TMenuItem& aMenuItem, CDesC16Array* aRequiredAttributes,
		CDesC16Array* aIgnoredAttributes, CLiwDefaultMap* aMap )
	{
    TInt pos( KErrNotFound );
    TBool exists = EFalse;
    TBuf<KMenuMaxAttrNameLen> running;
    running.Copy( KRunning );
    TBuf<KMenuMaxAttrNameLen> bitmap;
    bitmap.Copy( KBitmapBuffer );
    TBuf<KMenuMaxAttrNameLen> mask;
    mask.Copy( KMaskBuffer );
    TBool findBitmap( EFalse );
    TBool findMask( EFalse );

	RBuf attrvalue;
	attrvalue.CreateL( KMenuMaxAttrValueLen );
	CleanupClosePushL( attrvalue );

    for( TInt i= 0; i < aRequiredAttributes->MdcaCount(); i++ )
        {
        if( aRequiredAttributes->MdcaPoint(i).Compare( running ) == KErrNone
            && aIgnoredAttributes->Find( running, pos ) != KErrNone )
            {
            if ( aMenuItem.Type() == KMenuTypeApp() || aMenuItem.Type() == KMenuTypeFolder() )
                {
                iEng.GetAttributeL( iId, KRunningStatus, exists, attrvalue );
                aMap->InsertL( KRunning, TLiwVariant( exists ) );
                }
            }
        else if( aRequiredAttributes->MdcaPoint(i).Compare( bitmap ) == KErrNone
                && aIgnoredAttributes->Find( bitmap, pos ) != KErrNone )
            {
            findBitmap = ETrue;
            }
        else if( aRequiredAttributes->MdcaPoint(i).Compare( mask ) == KErrNone
                && aIgnoredAttributes->Find( mask, pos ) != KErrNone )
            {
            findMask = ETrue;
            }
        else if( aIgnoredAttributes->Find( aRequiredAttributes->MdcaPoint(i), pos ) != KErrNone )
            {
            iEng.GetAttributeL(
                    iId, aRequiredAttributes->MdcaPoint(i), exists, attrvalue );
            if( exists )
                {
                TBuf8<KMenuMaxAttrNameLen> attrname;
                attrname.Copy( aRequiredAttributes->MdcaPoint(i) );
                aMap->InsertL( attrname, TLiwVariant( attrvalue ) );
                }
            }
        }
    if( findBitmap && findMask )
        {
        CLiwDefaultMap* iconAttrMap = CLiwDefaultMap::NewLC();
       	FillIconAttributesMapL( aMenuItem, iconAttrMap );
       	aMap->InsertL( KIconAttributes, TLiwVariant( iconAttrMap ) );
       	CleanupStack::PopAndDestroy( iconAttrMap );
        }
	CleanupStack::PopAndDestroy( &attrvalue );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::AddAllAttributesL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::AddAllAttributesL(
		const TMenuItem& aMenuItem, CLiwDefaultMap* aMap )
	{
	TBool exists = EFalse;
    RBuf attrvalue;
	attrvalue.CreateL( KMenuMaxAttrValueLen );
	CleanupClosePushL( attrvalue );

	if ( aMenuItem.Type() == KMenuTypeApp() ||
			aMenuItem.Type() == KMenuTypeFolder() )
	    {
        iEng.GetAttributeL( iId, KRunningStatus, exists, attrvalue );
        aMap->InsertL( KRunning, TLiwVariant( exists ) );
	    }
	RArray<TPtrC> attrNameList;
	CleanupClosePushL(attrNameList);
    iEng.GetAttributeListL( aMenuItem.Id(), attrNameList );

	for(TInt i = 0;i< attrNameList.Count();i++)
	    {
	    TBuf8<KMenuMaxAttrNameLen> attrname;
	    attrname.Copy( attrNameList[i] );
        iEng.GetAttributeL( iId, attrNameList[i], exists, attrvalue );
	    if(exists)
	        {
	        attrname.Copy(attrNameList[i]);
	        aMap->InsertL( attrname, TLiwVariant( attrvalue ) );
	        }
	    }
    CLiwDefaultMap* iconAttrMap = CLiwDefaultMap::NewLC();
   	FillIconAttributesMapL( aMenuItem, iconAttrMap );
   	aMap->InsertL( KIconAttributes, TLiwVariant( iconAttrMap ) );
   	CleanupStack::PopAndDestroy( iconAttrMap );
	CleanupStack::PopAndDestroy(&attrNameList);
	CleanupStack::PopAndDestroy( &attrvalue );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::FillIconAttributesMapL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::FillIconAttributesMapL(
		const TMenuItem& aMenuItem, CLiwDefaultMap* aMap )
	{

    if ( aMenuItem.Type() == KMenuTypeApp())
    	{
    	AppendAttrL( KMenuAttrUid, aMap );
    	AppendAttrL( KMenuAttrNative, aMap );
    	}
    else if ( aMenuItem.Type() == KMenuTypeFolder() )
    	{
    	AppendAttrL( KChildrenCount, aMap );
    	}
	AppendAttrL( KMenuAttrIconSkinMajorId, aMap );
	AppendAttrL( KMenuAttrIconSkinMinorId, aMap );
	AppendAttrL( KMenuAttrIconFile, aMap );
	AppendAttrL( KMenuAttrIconId, aMap );
	AppendAttrL( KMenuAttrMaskId, aMap );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::AppendAttrL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::AppendAttrL(
        const TDesC& aAttrName, CLiwDefaultMap* aMap )
	{
	TBool exists;
	TBuf8<KMenuMaxAttrNameLen> attrName;
	RBuf attrvalue;
    attrvalue.CreateL( KMenuMaxAttrValueLen );
	CleanupClosePushL( attrvalue );
    iEng.GetAttributeL( iId, aAttrName, exists, attrvalue );
	if ( exists )
		{
		attrName.Copy( aAttrName );
		aMap->InsertL(attrName, TLiwVariant( attrvalue ) );
		}
	CleanupStack::PopAndDestroy( &attrvalue );
	}

// ---------------------------------------------------------
// CMcsGetListCreatorInterface::SetGetterStrategyL
// ---------------------------------------------------------
//
void CMcsGetListCreatorInterface::SetItemIdL( TInt aId )
	{
	iId = aId;
	}
//  End of File
