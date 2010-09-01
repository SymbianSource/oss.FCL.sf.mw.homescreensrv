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


#include <s32mem.h>
#include <AknIconUtils.h>

#include "mcsmenu.h"
#include "mcsmenusapi.h"
#include "mcsmenuitem.h"
#include "mcsgetlistimpl.h"
#include "mcsservice.h"
#include "mcsoperationmanager.h"
#include "mcsiconutility.h"
#include "mcsbitmapbuffer.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSGetListImpl* CMCSGetListImpl::NewL( RMenuSapi& aMCS, MMCSCallback* aCallback,
                                     RMcsOperationManager& aOperationManager )
	{
	CMCSGetListImpl* self = new (ELeave) CMCSGetListImpl( aMCS, aCallback, 
	                                                    aOperationManager );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSGetListImpl::~CMCSGetListImpl()
	{
	Cancel();
	iSerializedInputList.Close();
	iSerializedOutputList.Close();
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSGetListImpl::CMCSGetListImpl( RMenuSapi& aMCS, MMCSCallback* aCallback,
                                RMcsOperationManager& aOperationManager ) :
						CActive( EPriorityStandard ),
						iCallback( aCallback ),
						iMCS( aMCS ),
						iOperationManager( aOperationManager )						
	{
	}

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSGetListImpl::ConstructL()
	{
	}

// ---------------------------------------------------------------------------
// Async
// ---------------------------------------------------------------------------
void CMCSGetListImpl::GetListL( const CLiwGenericParamList& aInParamList )
    {
    if( !iCallback )
        {
        User::Leave( KErrArgument );
        }
 
    if (!IsActive())
    	{
		iSerializedInputList.Close( );
		iSerializedInputList.CreateL( aInParamList.Size() );
		
		RDesWriteStream datastrm( iSerializedInputList );
		CleanupClosePushL(datastrm);
		aInParamList.ExternalizeL(datastrm);
		datastrm.CommitL();
		CleanupStack::PopAndDestroy(&datastrm);
	
		CActiveScheduler::Add ( this );
		ActivateRequest( KErrNone );
    	}
    }

// ---------------------------------------------------------------------------
// Sync
// ---------------------------------------------------------------------------
void CMCSGetListImpl::GetListL( )
    {
    iSerializedOutputList.Close();
    iMCS.GetListL( iSerializedInputList, iSerializedOutputList );
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSGetListImpl::DoCancel()
	{
//	TRequestStatus* temp = &iStatus;
//	User::RequestComplete( temp, KErrCancel );
	TRAP_IGNORE( iCallback->NotifyResultL( KErrCancel, NULL ));
//	NotifyRequestResult( KErrCancel );
	}

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSGetListImpl::RunL()
	{
	TInt err = iStatus.Int();

    if ( err == KErrNone )
        {
        TRAP( err, GetListL( ));
        }       
    NotifyRequestResultL( err );
	}

// ---------------------------------------------------------------------------
// CMCSGetListImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSGetListImpl::RunError(TInt aError)
	{
    TRAP( aError,NotifyRequestResultL( aError ) );
    if ( (aError != KErrDiskFull) && (aError != KErrNoMemory))
    	{
    	aError = KErrNone;
    	}    
	return aError; 
	}
// ---------------------------------------------------------------------------
// Activates the asynchronous request
// ---------------------------------------------------------------------------
//
void CMCSGetListImpl::ActivateRequest( TInt aReason )
	{
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* temp = &iStatus;
    User::RequestComplete( temp, aReason );
	}

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSGetListImpl::NotifyRequestResultL( TInt aReason )
    {
    iOperationManager.Remove( this );//remove it 
                                     //from operation manager   
    
    if ( iCallback )
        {
    	CLiwGenericParamList* outParamList = CLiwGenericParamList::NewLC();
		
		if ( aReason == KErrNone )
			{
			TRAP(aReason, HandleAppendIconsL( outParamList ));
			}
			
        TRAP_IGNORE(iCallback->NotifyResultL( aReason, (TAny*)(outParamList)));
		
        CleanupStack::PopAndDestroy( outParamList );

        // iCallback deletes itself.
        }
    delete this;
    }
	
// --------------------------------------------------------------------------------------------------------
// Appends icons to generic param list
// --------------------------------------------------------------------------------------------------------
//
void CMCSGetListImpl::HandleAppendIconsL( CLiwGenericParamList* aParamList )
	{
	RDesReadStream stream( iSerializedOutputList );
    CleanupClosePushL( stream );
   	aParamList->InternalizeL( stream ) ;
   	TInt pos ( 0 );
   	const TLiwGenericParam* returnVal = 
					aParamList->FindFirst( pos, KReturnValue );
   	if ( returnVal )
   		{
   		CLiwDefaultList* resultList = CLiwDefaultList::NewLC();
   		const CLiwList* resultTemp = returnVal->Value().AsList();
   		AppendIconsL( resultTemp , resultList );
   		aParamList->Remove( KReturnValue );
   		aParamList->AppendL( TLiwGenericParam(KReturnValue, 
						TLiwVariant( resultList ) ) );
       	CleanupStack::PopAndDestroy( resultList );
       	}
	CleanupStack::PopAndDestroy( &stream );
	}
	
// --------------------------------------------------------------------------------------------------------
// Appends icons to generic param list
// --------------------------------------------------------------------------------------------------------
//
void CMCSGetListImpl::AppendIconsL( const CLiwList* aList,
    CLiwDefaultList* aDefaultList )
	{
    for ( TInt index = 0; index < aList->Count(); index++ )
		{
		CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
		TLiwVariant variant;
		variant.PushL();
		aList->AtL( index, variant );
		variant.Get( *map );
		AppendIconL( map );
		aDefaultList->AppendL( TLiwVariant( map ) );
		CleanupStack::PopAndDestroy( &variant );
		CleanupStack::PopAndDestroy( map );
		}
		
	}

// --------------------------------------------------------------------------------------------------------
// Appends icons to specific map
// --------------------------------------------------------------------------------------------------------
//
void CMCSGetListImpl::AppendIconL( CLiwDefaultMap* aMap )
	{
	//check if there are any children and call AppendIconsL recursively if so
    TLiwVariant variant;
    variant.PushL();
	if( aMap->FindL( KChildren, variant ) )
	    {
	    const CLiwList* tempList = variant.AsList();
		CLiwDefaultList* resultList = CLiwDefaultList::NewLC();
		AppendIconsL( tempList, resultList );
		aMap->Remove( KChildren );
		aMap->InsertL( KChildren, TLiwVariant( resultList ) ) ;
		CleanupStack::PopAndDestroy( resultList );
	    }

	if( aMap->FindL( KIconAttributes, variant ) )
		{
		CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
		variant.Get( *map );
	    CAknIcon* icon = NULL;
        TRAP_IGNORE( icon = McsIconUtility::GetItemIconL( map )  );
        aMap->Remove( KIconAttributes );
        if ( icon )
               { 
               CleanupStack::PushL( icon ); 
               GetMaskAndBitmapL( *aMap, icon );
               CleanupStack::PopAndDestroy( icon );           
               }
		CleanupStack::PopAndDestroy( map );
		}

	CleanupStack::PopAndDestroy( &variant );
	}

// ---------------------------------------------------------------------------
// CMCSGetListImpl::GetMaskAndBitmapL.
// ---------------------------------------------------------------------------
//
void CMCSGetListImpl::GetMaskAndBitmapL( CLiwDefaultMap& aMap, CAknIcon* aIcon )
    {
    CLiwBitmapBuffer* bitmapBuffer = CMcsBitmapBuffer::NewL( aIcon->Bitmap() );
    aIcon->SetBitmap( NULL ); 
    bitmapBuffer->PushL();               
    TLiwVariant lvBitmap( bitmapBuffer );
    lvBitmap.PushL();
    aMap.InsertL( KBitmapBuffer, lvBitmap );
    CleanupStack::PopAndDestroy( &lvBitmap ); 
    CleanupStack::PopAndDestroy( bitmapBuffer ); 


    CLiwBitmapBuffer* maskBuffer = CMcsBitmapBuffer::NewL( aIcon->Mask() );
    aIcon->SetMask( NULL );
    maskBuffer->PushL();
    TLiwVariant lvMask( maskBuffer );
    lvMask.PushL();
    aMap.InsertL( KMaskBuffer, lvMask );
    CleanupStack::PopAndDestroy( &lvMask );     
    CleanupStack::PopAndDestroy( maskBuffer ); 
    }
