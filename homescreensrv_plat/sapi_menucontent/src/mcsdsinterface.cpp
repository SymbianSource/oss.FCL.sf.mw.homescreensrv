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
 

#include <e32base.h>
#include <e32def.h>
#include "mcsdef.h"
#include "mcsmenufilter.h"
#include "mcsmenuitem.h"
#include "mcsmenunotifier.h"

#include "serviceerrno.h"
#include "mcsservice.h"
#include "mcsdsinterface.h"
#include "mcscallback.h"
#include "mcsconstants.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMCSDSInterface* CMCSDSInterface::NewL(const TDesC& aContent)
	{
  	CMCSDSInterface* self = new (ELeave) CMCSDSInterface();
  	CleanupStack::PushL( self );
  	self->ConstructL(aContent);
  	CleanupStack::Pop( self );
	return self;
	}
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//	
CMCSDSInterface::~CMCSDSInterface()
	{
	delete iMCSService;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMCSDSInterface::CMCSDSInterface()
	{
	}

// ---------------------------------------------------------------------------
// Symbian Constructor
// ---------------------------------------------------------------------------
//	
void CMCSDSInterface::ConstructL(const TDesC& aContent)
	{
	iMCSService = CMCSService::NewL(aContent);	
	}

// ---------------------------------------------------------------------------
//  Executes the SAPI as per params
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::ExecuteCmdL( const TDesC8& aCmdName,
					             const CLiwGenericParamList& aInParamList,
					             CLiwGenericParamList& aOutParamList,
					             TUint aCmdOptions,
					             MLiwNotifyCallback* aCallback )
	{
	TInt errcode = KErrNotSupported;
	TInt32 transactionId = -1; 
		
	aOutParamList.AppendL(TLiwGenericParam( KErrorCode, 
							TLiwVariant( ErrCodeConversion( KErrNone ))));		
	
	if ( !aCallback )
		{
		// Callback is a null pointer
		errcode = KErrArgument;
		}
	else
		{
	
		if ( aCmdName.CompareF( KCmdAdd ) == 0 ) 
			{
			TRAP(errcode, AddItemL( aInParamList, aCallback ));
			}
		else if ( aCmdName.CompareF( KCmdDelete ) == 0 ) 
			{
			  TRAP(errcode, DeleteL( aInParamList, aCallback ));
			}
		else if ( aCmdName.CompareF( KCmdGetList ) == 0 ) 
			{
			TRAP(errcode, GetListL( aInParamList, aCallback ));
			}
		else if ( aCmdName.CompareF( KCmdReqNotification ) == 0 ) 
			{
			if( aCallback && !(aCmdOptions & KLiwOptCancel) )
	            {
	            TRAP(errcode, RequestNotificationL(aInParamList, aCallback));
	            }
	        if ( aCmdOptions & KLiwOptCancel )
	            {
	            TRAP(errcode, iMCSService->UnRegisterObserverL());
	            }
			
			}
		
		// Append transaction id in case of asynchronous requests 
		if( ( KLiwOptASyncronous & aCmdOptions ) &&  
							( errcode == KErrNone ) && ( transactionId != -1 ) )
			{
			aOutParamList.AppendL(TLiwGenericParam( KTransactionID, 
									TLiwVariant( TInt32( transactionId ))));		
			}	
		}
	
	if( errcode != KErrNone )
		{
		aOutParamList.Reset();
		aOutParamList.AppendL(TLiwGenericParam( KErrorCode, 
									TLiwVariant(ErrCodeConversion(errcode))));		
		}
	}

// ---------------------------------------------------------------------------
// Closes the interface
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::Close()
	{
	delete this;
	}

// ---------------------------------------------------------------------------
// Issues Add a new Item request to MCSService
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::AddItemL( const CLiwGenericParamList& aInParamList, 
                              MLiwNotifyCallback* aCallback )
    {
    TInt32 itemId = 0;
    TInt32 parentId = 0;
    TInt32 beforeId = 0;    
    TInt pos = 0;
    TPtrC paramvalue;

    TBuf16<KMenuMaxAttrValueLen> paramValStr;
    TBuf16<KMenuMaxAttrNameLen> attrname16;
    TBuf8<KMenuMaxAttrNameLen> attrname;
    TBuf16<KMenuMaxAttrValueLen> paramType;
    TLiwVariant param;    
    
    const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KInData );
    if (inParam)
        {
        CLiwMap* inputMap = (CLiwMap*)inParam->Value().AsMap();
        if ( inputMap )
           {
           attrname.Copy(KMenuAttrId());
           if( inputMap->FindL( attrname, param))
        	   {
        	   param.Get(itemId);
        	   inputMap->Remove( attrname );
        	   }
           param.Reset();
           if( inputMap->FindL( KParentId, param))
        	   {
        	   param.Get(parentId);
        	   inputMap->Remove( KParentId );
        	   }
           param.Reset();

           if( inputMap->FindL( KBeforeId, param))
        	   {
        	   param.Get(beforeId);
        	   inputMap->Remove(KBeforeId);
        	   }
           param.Reset();
           if( inputMap->FindL( KType, param))
        	   {
        	   paramType.Copy(param.AsDes());
        	   inputMap->Remove( KType ); 
        	   }
           param.Reset();
           
           CMCSCallback* cb = CMCSCallback::NewL(aCallback, aInParamList, 
                   aCallback->GetTransactionID(), *iMCSService,CMCSCallback::EAddORChange );
           CleanupStack::PushL(cb);
           iMCSService->UpdateItemL(cb, paramType, itemId, parentId, beforeId, *inputMap);
           CleanupStack::Pop(cb);
           }
        else
        	{
        	User::Leave(KErrArgument);
        	}
        }
    else
        {             
       User::Leave(KErrArgument);
        }
    }
           
// ---------------------------------------------------------------------------
// Issues GetList request to MCSService
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::GetListL( const CLiwGenericParamList& aInParamList, 
		                      MLiwNotifyCallback* aCallback )
	{
	CheckGetListInputListL( aInParamList );
    CMCSCallback* cb = CMCSCallback::NewL(aCallback, aInParamList, 
    		aCallback->GetTransactionID(), *iMCSService,CMCSCallback::EGetList );
    CleanupStack::PushL(cb);
    iMCSService->GetListL( aInParamList, cb );
    CleanupStack::Pop(cb);
	}
// ---------------------------------------------------------------------------
// Check if inputlist for getlist is proper
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::CheckGetListInputListL( const CLiwGenericParamList&
			aInParamList )
	{
	TInt pos( 0 );
	const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KInData );
	if (inParam)
		{
		const CLiwMap* inputMap = inParam->Value().AsMap();
		if ( inputMap )
	        {
	        TLiwVariant param;
	        // Folder Id
	        if( !inputMap->FindL( KMenuAttrId8, param ) )
	                {
	            	User::Leave( KErrArgument );
	                }
            }
	    else
		   	{
        	User::Leave( KErrArgument );
		   	}
		}
	else
		{
    	User::Leave( KErrArgument );
		}
	}

// ---------------------------------------------------------------------------
// Issues Change Notification request to MCSService
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::RequestNotificationL( const CLiwGenericParamList& aInParamList, 
					                       			MLiwNotifyCallback* aCallback)
													
	{
	TInt pos = 0;
	TInt events = 0;
    TBuf8<KMenuMaxAttrNameLen> attrname;
    TInt paramFolderId = 0;
    TLiwVariant param;
	const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KInData );
	if (inParam)
	        {
	        const CLiwMap* inputMap = inParam->Value().AsMap();
	        if ( inputMap )
	            {
	            // Folder Id
	            attrname.Copy(KMenuAttrId());
	            if( inputMap->FindL( attrname, param))
	                {
	                paramFolderId = param.AsTInt32();
	                }
	            param.Reset();
	            if( inputMap->FindL( KAddRemove, param))
                   {
                   if (param.AsTBool())
                       {
                        events =  events  | RMenuNotifier::EItemsAddedRemoved;
                       }
                   }
                param.Reset();
                if( inputMap->FindL( KReorder, param))
                   {
                   if (param.AsTBool())
                      {
                       events =  events  | RMenuNotifier::EItemsReordered;
                      }
                   }
                param.Reset();
                if( inputMap->FindL( KAttributeChange, param))
                   {
                   if (param.AsTBool())
                     {
                      events =  events  | RMenuNotifier::EItemAttributeChanged;
                     }
                   }
                param.Reset();
	            }
	        }
	
	CMCSCallback* cb = CMCSCallback::NewL(aCallback, aInParamList, 
	                aCallback->GetTransactionID(), *iMCSService,  CMCSCallback::ERequestNotification );
	CleanupStack::PushL( cb );
		
	// Issue request
	if(events)
		{
		iMCSService->RegisterObserverL(paramFolderId, events, cb );
		}
	else
		{
        User::Leave(KErrArgument);
		}
	CleanupStack::Pop( cb );
	}

// ---------------------------------------------------------------------------
// Issues Delete an item  request to MCSService
// ---------------------------------------------------------------------------
//
void CMCSDSInterface::DeleteL( const CLiwGenericParamList& aInParamList, 
                              MLiwNotifyCallback* aCallback )
    {
    TInt pos = 0;    
    TBuf8<KMenuMaxAttrNameLen> attrname;
    TInt itemId = -1;
    TLiwVariant param;
    const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KInData );
    if (inParam)
            {
            const CLiwMap* inputMap = inParam->Value().AsMap();
            if ( inputMap )
                {
                // Item Id
                attrname.Copy(KMenuAttrId());
                if( inputMap->FindL( attrname, param))
                    {
                    itemId = param.AsTInt32();
                    }
                param.Reset();
                }
            }

    if (itemId == -1)
        {
        User::Leave(KErrArgument);
        }
    else
        {
        CMCSCallback* cb = CMCSCallback::NewL(aCallback, aInParamList, 
                aCallback->GetTransactionID(), *iMCSService,CMCSCallback::EDelete );
        CleanupStack::PushL(cb);
    
        iMCSService->DeleteL( cb, itemId );
        CleanupStack::Pop(cb);
        }
    }


// ---------------------------------------------------------------------------
// ErrCode Conversion
// ---------------------------------------------------------------------------
//
TInt32 CMCSDSInterface::ErrCodeConversion(TInt code)
	{
	TInt32 err;
	switch (code)
		{
		case KErrCancel:
						// Returning KErrNone incase of KErrCancel
		case KErrNone:
			err= SErrNone;
			break;

		case KErrNotFound:
			err= SErrNotFound;
			break;

		case KErrNoMemory:
			err = SErrNoMemory;
			break;

		case KErrInUse:
			err = SErrServiceInUse;
			break;

		case KErrNotSupported:
			err = SErrServiceNotSupported;
			break;

		case KErrBadName:
			err = SErrBadArgumentType;
			break;
						
		case KErrArgument: 
			err = SErrInvalidServiceArgument;
			break;
			
		case KErrAccessDenied: 
				err = SErrAccessDenied;
				break;
	
		default:
			err = SErrGeneralError;
			break;
		}
	
    return err;
	}

// End of file
