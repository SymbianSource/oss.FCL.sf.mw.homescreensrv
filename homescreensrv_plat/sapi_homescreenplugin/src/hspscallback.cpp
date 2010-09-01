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

#include "hspscallback.h"
#include "hspsconfigurationservice.h"
#include "hspsliwvocabulary.hrh"
#include "hspsserviceutilities.h"
#include <hspsdefinitionrepository.h>

// ---------------------------------------------------------------------------
// Two-phase Constructor
// ---------------------------------------------------------------------------
//
CHspsReqNotifCallback* CHspsReqNotifCallback::NewL( MLiwNotifyCallback *aPtrNotifyCallback, 
                                  const CLiwGenericParamList& aPtrInParamList, 
                                  TInt32 aTransactionId, CHspsConfigurationService& aHspsConfigurationService  )
    {
    return new (ELeave)    CHspsReqNotifCallback( aPtrNotifyCallback, aPtrInParamList, aTransactionId,
        aHspsConfigurationService );
    }
    
// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//    
CHspsReqNotifCallback::CHspsReqNotifCallback( MLiwNotifyCallback *aPtrNotifyCallback, 
                            const CLiwGenericParamList& aPtrInParamList, 
                            TInt32 aTransactionId, CHspsConfigurationService& aHspsConfigurationService )
    {
    iPtrNotifyCallback = aPtrNotifyCallback; 
    iPtrInParamList = &aPtrInParamList;
    iTransactionId = aTransactionId;
    iHspsConfigurationService = &aHspsConfigurationService;
    }

// ---------------------------------------------------------------------------
//  Destructor
// ---------------------------------------------------------------------------
//  
CHspsReqNotifCallback::~CHspsReqNotifCallback()
    {
    
    }
// ---------------------------------------------------------------------------
// Gives the result of asynchronous SAPI calls
// ---------------------------------------------------------------------------
//
void CHspsReqNotifCallback::NotifyResultL( TInt aErrCode,
        ChspsRequestNotificationParams& aParams, RArray<TInt>& aIds )
    {
    


    CLiwGenericParamList* outParams = CLiwGenericParamList::NewL();

    CleanupStack::PushL( outParams );
                            
    
    if (  aErrCode == KErrNone )
        {
        CLiwDefaultMap* notifMap = CLiwDefaultMap::NewL();       
        CleanupStack::PushL( notifMap );
        
        
        TPtrC8 event = ConvertEvent2LiwData(aParams.iEvent);
         
        notifMap->InsertL( KReqNotifEvent, TLiwVariant( event ) );
       
        if( aParams.iAppConfUid )
            {
            TBuf8<10> appConfUid;
            appConfUid.Append( '0' );
            appConfUid.Append( 'x' );
            appConfUid.AppendNum( aParams.iAppConfUid, EHex );
                 
            notifMap->InsertL( KHspsLiwAppConfUid, TLiwVariant( appConfUid ) );
            }
        else
            {
            notifMap->InsertL( KHspsLiwAppConfUid, TLiwVariant( KHspsLiwEmptyValue ) );
            }

        if( aParams.iOrigUid )
            {
            TBuf8<10> origUid;
            origUid.Append( '0' );
            origUid.Append( 'x' );
            origUid.AppendNum( aParams.iOrigUid, EHex );
                     
            notifMap->InsertL( KHspsLiwOrigUid, TLiwVariant( origUid ) );
            }
        else
            {
            notifMap->InsertL( KHspsLiwOrigUid, TLiwVariant( KHspsLiwEmptyValue ) );
            }

        if( aParams.Name().Length() > 0 )
            {
            TBufC<255> buf(aParams.Name());
            TDesC& name16 = const_cast<TDesC&>(aParams.Name());            
            //name ( 16 bit string to 8 bit string )
            HBufC8* name8 = HBufC8::NewL(  name16.Length() );
            CleanupStack::PushL( name8 );
            TPtr8 namePtr = name8->Des();
            namePtr.Copy( aParams.Name() );
        
            notifMap->InsertL( KHspsLiwName, TLiwVariant(  namePtr ));
            CleanupStack::PopAndDestroy( name8 );
            }
        else
            {
            notifMap->InsertL( KHspsLiwName, TLiwVariant(  KHspsLiwEmptyValue ));
            }
        if( aParams.iPluginUid )
            {
            TBuf8<10> pluginUid;
            pluginUid.Append( '0' );
            pluginUid.Append( 'x' );
            pluginUid.AppendNum( aParams.iPluginUid, EHex );
                     
            notifMap->InsertL( KHspsLiwPluginUid, TLiwVariant( pluginUid ) );
            }
        else
            {
            notifMap->InsertL( KHspsLiwPluginUid, TLiwVariant(  KHspsLiwEmptyValue ));
            }
         
        CLiwDefaultList* list = CLiwDefaultList::NewL();
        CleanupStack::PushL( list );
        
       
        for(TInt i = 0; i< aIds.Count(); i++)
            {
            TBuf8<10> pluginId;
            pluginId.AppendNum( aIds[i], EDecimal );
            list->AppendL(TLiwVariant( pluginId ));
            }
        notifMap->InsertL(KHspsLiwPluginIds, list);
        
        
        
        TLiwVariant rootMapVariant;
        rootMapVariant.Set( notifMap );
        //Put iterable into outParamList
        TLiwGenericParam rootMapParam;
        rootMapParam.SetNameAndValueL( KHspsLiwNotification, rootMapVariant );
        outParams->AppendL( rootMapParam );
        CleanupStack::Pop( list );
        list->DecRef(); 
                
        CleanupStack::Pop( notifMap );
        notifMap->DecRef();
        rootMapParam.Reset();
        rootMapVariant.Reset();  
        }
    else
        {
        outParams->AppendL( TLiwGenericParam(KHspsLiwStatus, 
                                        TLiwVariant((TInt32) aErrCode )));
        }
        
    //KLiwEventCompleted is used currently since our test cases and system
    //are not supported  KLiwEventInProgress type of notification mechanism
    TInt event = KLiwEventInProgress;
    if ( aErrCode == KErrCancel )
        {
   //     event = KLiwEventCanceled;
        }
    else if ( aErrCode != KErrNone )
        {
   //     event = KLiwEventStopped;
         
        outParams->AppendL( TLiwGenericParam(KErrorMessage, 
                 TLiwVariant(_L("HSPS:RequestNotification:Notification stopped"))));
        }
     
    ((MLiwNotifyCallback*)iPtrNotifyCallback)->HandleNotifyL( iTransactionId, 
                                                 event, 
                                                 *outParams, 
                                                 *((CLiwGenericParamList*)iPtrInParamList) ); 
                                                 
    CleanupStack::PopAndDestroy( outParams ); 
       
    }
// ---------------------------------------------------------------------------
// Convert plugin Registry notification event to LIW format
// ---------------------------------------------------------------------------
//
const TDesC8& CHspsReqNotifCallback::ConvertEvent2LiwData( TInt aEvent )
    {
   
    if( aEvent == EhspsODTAdded )
        {
        return KReqNotifPluginInstalledEvent;
        }
    else if( aEvent == EhspsODTUpdated )
        {
        return KReqNotifPluginUpdatedEvent;
        }
    else if( aEvent == EhspsODTRemoved )
        {
        return KReqNotifRemovePluginEvent;
        }
    else if( aEvent == EhspsODTModified )
        {
        return KReqNotifAddPluginEvent;
        }
    else if( aEvent == EhspsSettingsChanged )
        {
        return KReqNotifSettingsChangedEvent;
        }
    else if( aEvent == EhspsODTActivated )
        {
        return KReqNotifAppConfActivetedEvent;
        }
    else if( aEvent == EhspsClean )
        {
        return KReqNotifPluginUnInstalledEvent;
        }
    else if ( aEvent == EhspsPluginActivated )
        {
        return KReqNotifPluginActivatedEvent;
        }
    else if ( aEvent == EhspsPluginReplaced )
        {
        return KReqNotifPluginReplacedEvent;
        }
    else
        {
        return KNullDesC8;
        }
    }

// End of file
