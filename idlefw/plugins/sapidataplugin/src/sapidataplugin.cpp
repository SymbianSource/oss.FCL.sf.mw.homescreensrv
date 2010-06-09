/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SAPI data plug-in publisher
*
*/

// System includes
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <PUAcodes.hrh>
#include <badesca.h>
#include <fbs.h>
#include <gulicon.h>
#include <AknsSkinInstance.h> 
#include <AknsUtils.h> 
#include <AknsConstants.h>
#include <e32property.h>

// User includes
#include <hspublisherinfo.h>
#include <aicontentobserver.h>
#include <aiutility.h>
#include <aipspropertyobserver.h>
#include <aipluginsettings.h>
#include <activeidle2domainpskeys.h>
#include <debug.h>

#include "sapidatapluginconst.h"
#include "sapidatapluginuids.hrh"
#include "sapidataplugin.h"
#include "sapidata.h"

// Constants
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidDataPlugin, CSapiDataPlugin::NewL ) 
    };

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) /
        sizeof( TImplementationProxy );
    return KImplementationTable;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CSapiDataPlugin* CSapiDataPlugin::NewL()
    {
    CSapiDataPlugin* self = new (ELeave) CSapiDataPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSapiDataPlugin::CSapiDataPlugin()
    : iNetworkStatus( EUnknown ), iPluginState( EStopped )
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::ConstructL()
    {     
    User::LeaveIfError( iRfs.Connect() );
    
    iData = CSapiData::NewL( this );   
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CSapiDataPlugin::~CSapiDataPlugin()
    {
    // deactivate the publishers
    delete iData;
    iObservers.Close();
    Release( iContent );
    iDataArray.ResetAndDestroy();

    if ( iContentModel )
        {
        for ( TInt i = iDataCount - 1; i >= 0  ; i-- )
            {
            User::Free((TAny*)iContentModel[i].cid);   
            }
        
        delete [] iContentModel;
        }
    
    iIconArray.Reset();
    iRfs.Close();
    }

// ---------------------------------------------------------------------------
// Publishes widget's texts and images
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::PublishL()
    {    
    TInt observers( iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( this );

    for ( TInt i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer = iObservers[i];
        
		if ( observer->StartTransaction( transactionId ) == KErrNone ) 				
            {
            // Publish all the data
    		iData->PublishL(observer, KAll );
    	    observer->Commit( transactionId );
            }

		// Release memory of the published text
        iDataArray.ResetAndDestroy();
        // Release memory of the published icons
        iIconArray.Reset();        
        }        
    }

// ---------------------------------------------------------------------------
// Publish a specific text to the widget  
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::PublishTextL(MAiContentObserver* aObserver, 
        TInt& aContentId, TDesC& aContentValue)
    {
    if ( aObserver->CanPublish( *this,  aContentId  ,  aContentId  ) )
        {
        if( aContentValue.Length() > 0 )
            {
            HBufC* contentText = HBufC::NewLC(aContentValue.Size());
            TPtr cDes = contentText->Des();
            cDes.Copy(aContentValue);
            aObserver->Publish( *this, aContentId, cDes, aContentId );  
            iDataArray.AppendL( contentText );
            CleanupStack::Pop( contentText );
            }
        else
            {
            aObserver->Clean( *this, aContentId, aContentId );
            }
        }
    }

// ---------------------------------------------------------------------------
// Publish a specific image to the widget  
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::PublishImageL(MAiContentObserver* aObserver,
        TInt aContentId, TDesC& aPath )
    {
    TInt err = KErrNone;
    TAknsItemID iconId;
    iconId.iMajor=0;
    iconId.iMinor=0;
    TInt bitmapId(0);
    TInt maskId(0);
    TFileName fileName;
    CGulIcon* icon = NULL;
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    
    if ( aObserver->CanPublish( *this, aContentId , aContentId ) )
      {
      TBool inSkin = iData->ResolveSkinIdAndMifId( aPath, iconId, bitmapId, maskId, fileName  );
      if ( inSkin )
          {
          // Load from skin 
          MAknsSkinInstance* skin = AknsUtils::SkinInstance();
          if ( iconId.iMajor != 0 && iconId.iMajor!=0 )
              {
              // Create icon with fall back 
              TRAP_IGNORE(AknsUtils::CreateIconL(
                      skin,
                      iconId,
                      bitmap,
                      mask,
                      fileName,  /* backup filename */
                      bitmapId,  /* backup bit map id */
                      maskId));   /* backup mask id */
              }
          else if( bitmapId !=0 )
              {
              if ( maskId!=0 )
                  {
                  // Create icon from Mif filename , bitmap id and mask id
                  TRAP_IGNORE(icon = AknsUtils::CreateGulIconL(
                          skin,
                          iconId,
                          fileName,
                          bitmapId,
                          maskId) );
                  }
              else
                  {
                  TRAP_IGNORE(AknsUtils::CreateIconL(
                          skin,
                          iconId,
                          bitmap,
                          fileName,  /* backup filename */
                          bitmapId)); /* backup bit map id */
                  }
              }
          
          if ( icon == NULL && bitmap !=  NULL )
              {
              icon = CGulIcon::NewL( bitmap, mask );
              }

          if ( icon != NULL ) // Syntax correct but icon not found
              {
              aObserver->PublishPtr( *this, aContentId, icon , aContentId );
              iIconArray.Append(icon);
              } 
          else
              {
              err = KErrNotFound;   
              aObserver->Clean( *this, aContentId, aContentId );
              }
          }
      else  // Interpret as File path
          {
          RFile iconFile;
          
          err = iconFile.Open( iRfs, aPath, EFileShareReadersOnly |  EFileRead );

          if( err == KErrNone )
            {
             aObserver->Publish( *this, aContentId, iconFile, aContentId );
            }
          else
              {
              aObserver->Clean( *this, aContentId, aContentId );
              }
          
          iconFile.Close();
          }
        }
    }

// ---------------------------------------------------------------------------
// Publish a image to the widget  
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::PublishImageL(MAiContentObserver* aObserver, 
    TInt& aContentId, TInt aHandle, TInt aMaskHandle )
    {
    if ( aObserver->CanPublish( *this, aContentId , aContentId ) )
        {
        if( aHandle != KErrBadHandle  )
            {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
            if( KErrNone == bitmap->Duplicate( aHandle) )
                {
                // Take the ownership
                CGulIcon* icon = CGulIcon::NewL(bitmap);
                if( aMaskHandle != KErrBadHandle )
                    {
                    CFbsBitmap* mask = new (ELeave) CFbsBitmap();
                    if (KErrNone == mask->Duplicate( aMaskHandle) )
                        {
                        icon->SetMask( mask );            
                        }
                    }
                aObserver->PublishPtr( *this, aContentId, icon , aContentId );
                iIconArray.Append(icon);
                }
            else
                {
                delete bitmap;
                bitmap = NULL;
                aObserver->Clean( *this, aContentId, aContentId );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Publish a data to the widget  
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::PublishData( MAiContentObserver* aObserver, 
    TInt& aContentId, const TDesC8& aData )
    {
    if ( aObserver->CanPublish( *this, aContentId, aContentId ) )
        {                
        aObserver->Publish( *this, aContentId, aData, aContentId );
        }    
    }

// ---------------------------------------------------------------------------
// Gets the id of a content  
// ---------------------------------------------------------------------------
//
TInt CSapiDataPlugin::GetIdL( TDesC& aObjectId)
	{
    TInt id = KErrNotFound;
	for( TInt i = 0;i<  iDataCount; i++ )
		{
		 if( aObjectId == ContentCid(iContentModel[i] ) )
			 {
			 id = iContentModel[i].id;
			 break;
			 }
		}
	return id;
	}
 
// ---------------------------------------------------------------------------
// Gets type of a content
// ---------------------------------------------------------------------------
//
const TDesC& CSapiDataPlugin::GetTypeL(TDesC& aObjectId )
	{
	for( TInt i = 0;i<  iDataCount; i++ )
		{
		 if( aObjectId ==  ContentCid(iContentModel[i]) )
			 {
			 if( iContentModel[i].type ==  KAiContentTypeText)
				 {
				 return KText();
				 }
			 else if( iContentModel[i].type == KAiContentTypeBitmap)
				 {
				 return KImage();
				 }
			 else if ( iContentModel[i].type == KAiContentTypeData )
			     {
                 return KData();
			     }
			 }
		}
	
	return KNullDesC();
	}

// ---------------------------------------------------------------------------
//Refresh a specific image or text in the widget
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::RefreshL( TDesC& aContentType,
                                TDesC& aOperation,
                                CLiwDefaultMap* aDataMap )
    {
    __PRINTS("*** CSapiDataPlugin::RefreshL ***");
    
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x, content type: %S, operation: %S" ),          
        &PublisherInfo().Name(), PublisherInfo().Uid().iUid, &aContentType, &aOperation ); 
        
    const TInt observerCount( iObservers.Count() );    
    const TInt transactionId = reinterpret_cast<TInt>( this );
    
    for( TInt obsIndex = 0; obsIndex < observerCount; obsIndex++ )
        {
        MAiContentObserver* observer = iObservers[obsIndex];
    
        if( observer->StartTransaction( transactionId ) == KErrNone ) 				
            {
            if ( aOperation != KOperationDelete )
                {
                if( aDataMap )
                    {
                    iData->PublishDataL( observer, aDataMap );
                    }
                else
                    {
                    iData->PublishL( observer, aContentType );
                    }            
                }
            else
                {
                iData->RemoveL( observer, aContentType  );
                }
    
            observer->Commit( transactionId );
            }
    
        // Relese memory of the published text
        iDataArray.ResetAndDestroy();
        iIconArray.Reset();
        }
    
    __PRINTS("*** CSapiDataPlugin::RefreshL - done ***");
    }

// ---------------------------------------------------------------------------
// Cleans a data from the widget
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::Clean(MAiContentObserver* aObserver, 
        TInt& aContentId )
	{
	 if ( aObserver->CanPublish( *this, aContentId, aContentId ) )
		{
		aObserver->Clean( *this, aContentId, aContentId );		
		}
	}

// ----------------------------------------------------------------------------
// CSapiDataPlugin::Start
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::Start( TStartReason aReason )
    {
    iPluginState = EStarted;
    
    TRAP_IGNORE( StartL( aReason ) );
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::StartL
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::StartL( TStartReason aReason )
    {       
    // Activate the publisher
    iData->ChangePublisherStatusL( KActive );
    
    if ( aReason == ESystemStartup )
        {
        PublishL();
        iData->SetStartupReasonL( KSystemStartup );
        }
    else if ( aReason == EPageStartup )
        {
        iData->SetStartupReasonL( KPageStartup );
        }
    else if ( aReason == EPluginStartup )
        {
        PublishL();
        iData->SetStartupReasonL( KPluginStartup );
        }

    if ( !iPubObsRegistered )
        {
        // Listen for publisher registration to resend the events 
        iData->RegisterPublisherObserverL() ;

        // Execute the active trigger 
        iData->TriggerActiveL();    
        
        iPubObsRegistered = ETrue;
        }    
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::Stop
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::Stop( TStopReason aReason )
    {
    TRAP_IGNORE( StopL( aReason ) );
    
    iPluginState = EStopped;
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::StopL
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::StopL( TStopReason aReason )
    {
    if ( aReason == ESystemShutdown )
        {
        iData->ChangePublisherStatusL( KSystemShutdown );
        }
    else if ( aReason == EPageShutdown )
        {
        iData->ChangePublisherStatusL( KPageShutdown );
        }
    else if ( aReason == EPluginShutdown )
        {
        iData->ChangePublisherStatusL( KPluginShutdown );
        }

    if ( iData )
        {
        iData->ChangePublisherStatusL( KDeActive );
        }          
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::Resume
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::Resume( TResumeReason aReason )
    {
    if ( aReason == EForeground && iPluginState != EStopped )
        {
        iPluginState = EResume;
        TRAP_IGNORE( iData->ChangePublisherStatusL( KResume ));
        }
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::Suspend
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::Suspend( TSuspendReason aReason )
    {
    if ( aReason == EBackground && iPluginState != EStopped )
        {
        iPluginState = ESuspend;
        TRAP_IGNORE( iData->ChangePublisherStatusL( KSuspend ));
        }    
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::SetOnline
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::SetOnline()
    {
    if ( iNetworkStatus != EOnline && iPluginState != EStopped )
        {
        iNetworkStatus = EOnline;
        TRAP_IGNORE( iData->ChangePublisherStatusL( KOnLine ));
        }
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::SetOffline
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::SetOffline()
    {
    if ( iNetworkStatus != EOffline && iPluginState != EStopped )
        {
        iNetworkStatus = EOffline;
        TRAP_IGNORE( iData->ChangePublisherStatusL( KOffLine ));
        }
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::SubscribeL
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::SubscribeL( MAiContentObserver& aObserver )
    {
    iObservers.AppendL( &aObserver );
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::ConfigureL
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    RAiSettingsItemArray contentItemsArr;    
    CleanupClosePushL( contentItemsArr );
    
    RAiSettingsItemArray configurationItemsArr;
    CleanupClosePushL( configurationItemsArr );
    
    TInt count( aSettings.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        MAiPluginSettings* setting( aSettings[i] );
        
        if ( setting->AiPluginItemType() == EAiPluginContentItem )
            {
            contentItemsArr.Append( setting );
            }
        else if ( setting->AiPluginItemType() == EAiPluginConfigurationItem )
            {
            configurationItemsArr.Append( setting );
            }
        }
    
    iDataCount = contentItemsArr.Count();
    
    // Create the content Model
    HBufC* contentId = HBufC::NewLC( 
        KAiContentIdMaxLength + KAiPluginNameMaxLength );
    
    iContentModel = new TAiContentItem[iDataCount];
    
    for ( TInt i = 0; i < iDataCount; i++ )
        {
        MAiPluginContentItem& contentItem( 
            contentItemsArr[i]->AiPluginContentItem() );
        
        iContentModel[i].id = i;
        
        const TDesC& type( contentItem.Type() );
        
        if ( type == KText() || type == KNewsTicker() ||                 
            type == KTextEditor() )            
            {
            // text
            iContentModel[i].type = KAiContentTypeText;
            }
        else if ( type == KImage() )
            {
            // image
            iContentModel[i].type = KAiContentTypeBitmap;
            }
        else if ( type == KData() )
            {
            // data stream 
            iContentModel[i].type = KAiContentTypeData;
            }
        
        contentId->Des().Copy( contentItem.Name() );
        contentId->Des().Delete( 0, 
            contentId->Des().LocateReverse( KPluginNameSeprator ) + 1 );

        TInt sizeOfContentId( contentId->Des().Size() + sizeof( wchar_t ) );
        
        iContentModel[i].cid = 
            static_cast< const wchar_t* >( User::Alloc( sizeOfContentId ) );
            
        Mem::Copy( ( TAny* )iContentModel[i].cid, 
            contentId->Des().PtrZ(), sizeOfContentId );
        
        contentId->Des().Delete( 0, contentId->Des().Length() );
        }    
    
    CleanupStack::PopAndDestroy( contentId );

    iContent = AiUtility::CreateContentItemArrayIteratorL( 
        iContentModel, iDataCount );

    iData->SetContentIdL( PublisherInfo().Namespace() );
    
    // Configurations 
    iData->ConfigureL( configurationItemsArr );

    // Listen the publisher content update
    iData->RegisterContentObserverL();              

    CleanupStack::PopAndDestroy( 2, &contentItemsArr ); // configurationItemsArr
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::SetProperty
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::SetProperty( TProperty aProperty, TAny* aAny )
    {
    if ( aProperty == ECpsCmdBuffer )
        {
        iData->SetCommandBuffer( aAny );
        }
    }
// ----------------------------------------------------------------------------
// CSapiDataPlugin::GetProperty
//
// ----------------------------------------------------------------------------
//
TAny* CSapiDataPlugin::GetProperty( TProperty aProperty )
    {    
    if ( aProperty == EPublisherContent )
        {
        return static_cast< MAiContentItemIterator* >( iContent );
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::HandleEvent
//
// ----------------------------------------------------------------------------
//
void CSapiDataPlugin::HandleEvent( const TDesC& aEventName, const TDesC& aParam )
    {
    TRAP_IGNORE( iData->ExecuteActionL( aEventName , aParam ) );    
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::HasMenuItem
//
// ----------------------------------------------------------------------------
//
TBool CSapiDataPlugin::HasMenuItem( const TDesC16& aMenuItem )
    {
    return iData->HasMenuItem ( aMenuItem );    
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::IsActive
//
// ----------------------------------------------------------------------------
//
TBool CSapiDataPlugin::IsActive() const
    {
    return iPluginState == EResume;
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::IsStopped
//
// ----------------------------------------------------------------------------
//
TBool CSapiDataPlugin::IsStopped() const
    {
    return iPluginState == EStopped;
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::Data
//
// ----------------------------------------------------------------------------
//
CSapiData* CSapiDataPlugin::Data() const
    {
    return iData;
    }

// ----------------------------------------------------------------------------
// CSapiDataPlugin::NetworkStatus
//
// ----------------------------------------------------------------------------
//
CSapiDataPlugin::TPluginNetworkStatus CSapiDataPlugin::NetworkStatus() const
    {
    return iNetworkStatus;
    }

// End of file

