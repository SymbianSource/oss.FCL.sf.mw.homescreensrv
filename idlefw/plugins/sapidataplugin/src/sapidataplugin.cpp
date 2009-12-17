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
* Description:  Profile plug-in publisher
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <aicontentobserver.h>
#include <aiutility.h>
#include <aipspropertyobserver.h>
#include <PUAcodes.hrh>
#include <aipluginsettings.h>
#include <badesca.h>
#include <fbs.h>
#include <gulicon.h>
#include <AknsSkinInstance.h> 
#include <AknsUtils.h> 
#include <AknsConstants.h>
#include <e32property.h>
#include <activeidle2domainpskeys.h>

#include "sapidatapluginconst.h"
#include "sapidatapluginuids.hrh"
#include "sapidataplugin.h"
#include "sapidata.h"

// CONST CLASS VARIABLES
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
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::ConstructL()
    { 
    iInfo.iUid.iUid = SAPIDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN; 
    iPluginState = ENone;
    iHSForeGround = EFalse;
    iNetworkStatus = EUnknown;
    iData = CSapiData::NewL(this);

    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CSapiDataPlugin::~CSapiDataPlugin()
    {
    // deactivate the publishers
    if( iData )
        {
        TRAP_IGNORE(iData->DeActivateL());
        delete iData;
        }
    iObservers.Close();
    Release( iContent );
    iDataArray.ResetAndDestroy();

    if( iContentModel)
        {
        for( TInt i = iDataCount-1;i>=0  ; i-- )
            {
            User::Free((TAny*)iContentModel[i].cid);   
            }
        delete []iContentModel;
        }
    iIconArray.Reset();
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-ins take ownership of the settings array, so it must either
// store it in a member or free it.
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    if( iDataCount > 0 )
        {
        // We own the array so destroy it
         aSettings.ResetAndDestroy();
         return;
        }
    
    RAiSettingsItemArray contentItemsArr;
    RAiSettingsItemArray configurationItemsArr;
    
    TInt count = aSettings.Count();
    for(TInt i = 0; i < count; i++ )
        {
        MAiPluginSettings* pluginSetting = aSettings[i];
        if( pluginSetting->AiPluginItemType() == EAiPluginContentItem )
            {
            contentItemsArr.Append(pluginSetting);
            }
        else if( pluginSetting->AiPluginItemType() == EAiPluginConfigurationItem )
            {
            configurationItemsArr.Append(pluginSetting);
            }
        }
    iDataCount = contentItemsArr.Count();
    if(iDataCount > 0 )
        {
        // Create the content Model
        HBufC* contentId = HBufC::NewLC( KAiContentIdMaxLength + KAiPluginNameMaxLength );
        iContentModel = new TAiContentItem[iDataCount];
        for(TInt i = 0; i < iDataCount; i++)
            {
            MAiPluginContentItem& contentItem = (contentItemsArr[i])->AiPluginContentItem();
            iContentModel[i].id = i;
            if( contentItem.Type() == KText() || contentItem.Type() == KNewsTicker() )
                {
                // text
                iContentModel[i].type = KAiContentTypeText;
                }
            if( contentItem.Type() == KImage() )
                {
                // image
                iContentModel[i].type = KAiContentTypeBitmap;
                }
            
            contentId->Des().Copy(contentItem.Name());
            contentId->Des().Delete(0, contentId->Des().LocateReverse(KPluginNameSeprator) +1);
  
            TInt sizeOfContentId = contentId->Des().Size()+sizeof(wchar_t);
            iContentModel[i].cid = static_cast<const wchar_t*>( User::Alloc( sizeOfContentId ) );
            Mem::Copy((TAny*)iContentModel[i].cid, contentId->Des().PtrZ(), sizeOfContentId);
            
            contentId->Des().Delete( 0, contentId->Des().Length());
            }    
        
        CleanupStack::PopAndDestroy( contentId );
        iContent = AiUtility::CreateContentItemArrayIteratorL( iContentModel, iDataCount );
        // Configurations 
        iData->ConfigureL(configurationItemsArr);
        
        // Activate the publisher
        iData->ActivateL();
                
        // Register for notifications
        iData->RegisterPublisherObserverL();
      
        PublishL();
        
        iPluginState = ESuspend;
        iData->RegisterContentObserverL();
        }
    contentItemsArr.Reset();
    configurationItemsArr.Reset();
       // We own the array so destroy it
    aSettings.ResetAndDestroy();
    // publish the initial data
    }

// ---------------------------------------------------------------------------
// Publishes widget's texts and images
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::PublishL()
    {
    TInt err( KErrNone );
    User::LeaveIfError( iRfs.Connect() );

    TInt observers( iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( this );

    for ( int i = 0; i < observers; i++ )
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
    iRfs.Close();
    }

// ---------------------------------------------------------------------------
// Publish a specific text of the widget  
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
// Publish a specific image of the widget  
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
          RFile* iconFile = new (ELeave) RFile();
          err = iconFile->Open( iRfs, aPath, EFileShareReadersOnly |  EFileRead );
          if( err == KErrNone )
            {
             aObserver->Publish( *this, aContentId, *iconFile, aContentId );
            }
          else
              {
              aObserver->Clean( *this, aContentId, aContentId );
              }
          iconFile->Close();
          delete iconFile;
          iconFile = NULL;
          }
        }
    }

// ---------------------------------------------------------------------------
// Publish a image of the widget  
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
			 }
		}
	
	return KNullDesC();
	}

// ---------------------------------------------------------------------------
//Refresh a specific image of text in the widget
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::RefreshL(TDesC& aContentType, TDesC& aOperation)
    {
     TInt err( KErrNone );
     User::LeaveIfError( iRfs.Connect() );
	 TInt observers( iObservers.Count() );        
	 TInt transactionId = reinterpret_cast<TInt>( this );
	 
	 for ( TInt obsIndex = 0; obsIndex < observers; obsIndex++ )
		{
		MAiContentObserver* observer = iObservers[obsIndex];
		
		if ( observer->StartTransaction( transactionId ) == KErrNone ) 				
			{
			if ( aOperation != KOperationDelete )
				{
				iData->PublishL( observer, aContentType );
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
	 iRfs.Close();
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
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is requested to unload its engines due backup operation
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::Stop( TAiTransitionReason aReason )
    {
    if( iPluginState == EResume )
        {
        Suspend( aReason );
        }
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::Resume( TAiTransitionReason aReason )
    {
    TRAP_IGNORE( DoResumeL( aReason ) ); 
    }
    
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is not allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::Suspend( TAiTransitionReason aReason )
    {
    switch( aReason )
		{
		case EAiKeylockDisabled:
        case EAiKeylockEnabled:
        	{
        	// handled in resume 
        	break;
        	}
        default :
        	{
        	iPluginState = ESuspend;
        	TRAP_IGNORE ( iData->SuspendL() );
        	}
		}
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// The plug-in MUST maintain a registry of subscribers and send 
// notification to all of them whenever the state changes or new content
// is available
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::SubscribeL( MAiContentObserver& aObserver )
    { 
    iObservers.AppendL( &aObserver );
    }
 

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Returns the extension interface. Actual type depends on the passed 
// aUid argument.
// ---------------------------------------------------------------------------
//
TAny* CSapiDataPlugin::Extension( TUid aUid )
    {    
    if ( aUid == KExtensionUidProperty )
   		{
        return static_cast<MAiPropertyExtension*>( this );
    	}
    else if (aUid == KExtensionUidEventHandler)
    	{
        return static_cast<MAiEventHandlerExtension*>( this );
    	}
    else
    	{	
        return NULL;
    	}
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Read property of publisher plug-in.
// ---------------------------------------------------------------------------
//
TAny* CSapiDataPlugin::GetPropertyL( TInt aProperty )
    {
    TAny* property = NULL;
    
    switch ( aProperty )
        {
    case EAiPublisherInfo:
        {
         property = static_cast<TAiPublisherInfo*>( &iInfo );
        break;  
        }       

    case EAiPublisherContent:
        {
        property = static_cast<MAiContentItemIterator*>( iContent );
        break;    
        }        
    default:
        break;
        }

    return property;
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Write property value to optimize the content model.
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::SetPropertyL( TInt aProperty, TAny* aValue )
    {  
    if( aProperty == EAiPublisherInfo )
        {
        ASSERT( aValue );
        
        const TAiPublisherInfo* info( 
                static_cast<const TAiPublisherInfo*>( aValue ) );
        
        iInfo = *info;
        
        iData->SetContentIdL( info->iNamespace );
        }
    }
 
// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::HandleEvent( TInt /*aEvent*/, const TDesC& /*aParam*/ )
	{
    // This is not as there is no event id to retrieve in this dynamic plugin. 
	} 
    
// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::HandleEvent( const TDesC& aEventName, const TDesC& aParam )
    {
    // We have no way of reporting errors to framework so just ignore them.
    TRAP_IGNORE(iData->ExecuteActionL( aEventName , aParam ) );
    }

// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Invoked by the framework for querying if plugin has menu item
// ---------------------------------------------------------------------------
//
TBool CSapiDataPlugin::HasMenuItem( const TDesC& aMenuItem )
	{ 
	return iData->HasMenuItem ( aMenuItem );  
	}

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// framework instructs plug-in that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CSapiDataPlugin::DoResumeL( TAiTransitionReason aReason )
    {
	//update in startup phase and idle is on foreground.
    switch ( aReason )
    	{
        case EAiIdleOnLine:
        	{
        	iNetworkStatus = EOnline;
			iData->OnLineL();
			break;
        	}
        case EAiIdleOffLine:
        	{
			iNetworkStatus = EOffline;
			iData->OffLineL();
			break;
			}
        case EAiIdlePageSwitch:
        	{
        	if ( iPluginState == EResume )
				{
				iData->SuspendL();
				}
        	iPluginState = EInActive;
        	iData->InActiveL();
        	}
        	break;
        case EAiSystemStartup:
    	case EAiIdleForeground:
    		{
			iHSForeGround = ETrue;
    		}
    	case EAiBacklightOn:    		
    		{
    		if ( iPluginState == ESuspend )
				{
				iPluginState = EResume;
				iData->ResumeL();
				}
    		break;
			}
    	case EAiKeylockDisabled:
        	{
        	// Key lock events considered only if HS is in foreground  
        	if ( iHSForeGround && iPluginState == ESuspend )
        		{
        		iPluginState = EResume;
				iData->ResumeL();
        		}
        	break;
        	}
    	case EAiKeylockEnabled:
        	{
        	// Key lock events considered only if HS is in foreground
        	if ( iHSForeGround && iPluginState == EResume )
        		{
				iPluginState = ESuspend ;
				iData->SuspendL();
        		}
        	break;
        	}
    	case EAiScreenLayoutChanged:
        	{
        	// ignore events
        	break;
        	}
      case EAiGeneralThemeChanged:
          {
          // ignore event
          break;
          }
    	case EAiIdleBackground: 
        	{
			iHSForeGround = EFalse;
        	}
        default :
        	{
			if ( iPluginState == EResume )
				{
				iPluginState = ESuspend;
				iData->SuspendL();
				}
        	break;
        	}
    	}
    }

// ---------------------------------------------------------------------------
// Is plugin active to publish the data 
// ---------------------------------------------------------------------------
//
TBool CSapiDataPlugin::IsActive()
    {
    return (iPluginState == EResume );
    }
