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
* Description:
* Profile plug-in publisher
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

#include "wrtdatapluginconst.h"
#include "wrtdatapluginuids.hrh"
#include "wrtdataplugin.h"
#include "wrtdata.h"

// CONST CLASS VARIABLES
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidDataPlugin, CWrtDataPlugin::NewL ) 
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
CWrtDataPlugin* CWrtDataPlugin::NewL()
    {
    CWrtDataPlugin* self = new (ELeave) CWrtDataPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWrtDataPlugin::CWrtDataPlugin()
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::ConstructL()
    { 
    iInfo.iUid.iUid = WRTDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN; 
    iPluginState = ENone;
    iHSForeGround = EFalse;
    iNetworkStatus = EUnknown;
    iData = CWrtData::NewL(this);

    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CWrtDataPlugin::~CWrtDataPlugin()
    {
    // deactivate the publishers
    if( iData )
        {
        if ( iPluginState != EInActive )
        	{
        	TRAP_IGNORE(iData->DeActivateL());
        	}
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
// Publishes widget's texts and images
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::PublishL()
    {
    TInt err( KErrNone );
    User::LeaveIfError( iRfs.Connect() );

    TInt observers( iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( this );

    for ( int i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer = iObservers[i];
                
        if ( observer->StartTransaction( transactionId ) == KErrNone )        	 
        	{// Publish all the data
            iData->PublishAllL(observer);
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
/*
void CWrtDataPlugin::PublishTextL(MAiContentObserver* aObserver, 
        TInt& aContentId, TDesC16& aContentValue)
    {
    if ( aObserver->CanPublish( *this,  aContentId  ,  aContentId  ) )
        {
        if( aContentValue.Length() > 0 )
            {
            HBufC* contentText = HBufC::NewLC(aContentValue.Size());
            TPtr16 cDes = contentText->Des();
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
    }*/

// ---------------------------------------------------------------------------
// Publish a specific image of the widget  
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::PublishImageL(MAiContentObserver* aObserver,
		TContentItem aContentId, TDesC16& aPath )
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
void CWrtDataPlugin::PublishImageL(MAiContentObserver* aObserver, 
		TContentItem aContentId, TInt aHandle, TInt aMaskHandle )
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
TInt CWrtDataPlugin::GetIdL( TDesC16& aObjectId)
	{
    TInt id = KErrNotFound;
    HBufC16* objectId = HBufC16::NewLC( KAiContentIdMaxLength );
	for( TInt i = 0;i<  iDataCount; i++ )
		{
		 objectId->Des().Copy((TUint16*)iContentModel[i].cid);
		 if( aObjectId == objectId->Des() )
			 {
			 id = iContentModel[i].id;
			 break;
			 }
		}
	CleanupStack::PopAndDestroy( objectId );
	return id;
	}
 

// ---------------------------------------------------------------------------
// Gets type of a content
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::GetTypeL(TDesC16& aObjectId, TDes16& aType )
	{
	HBufC16* objectId = HBufC16::NewLC( KAiContentIdMaxLength );
	for( TInt i = 0;i<  iDataCount; i++ )
		{
		objectId->Des().Copy((TUint16*)iContentModel[i].cid);
		 if( aObjectId == objectId->Des() )
			 {
			 if( iContentModel[i].type ==  KAiContentTypeText)
				 {
				 aType.Copy( KText );
				 }
			 else if( iContentModel[i].type == KAiContentTypeBitmap)
				 {
				 aType.Copy( KImage );
				 }
			 break;
			 }
		}
	CleanupStack::PopAndDestroy( objectId );
	}

// ---------------------------------------------------------------------------
//Refresh a specific image of text in the widget
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::RefreshL(TDesC16& aOperation)
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
            if( aOperation != KOperationDelete)
                {
                iData->PublishL( observer );
                }
            else
                {
                Clean( observer , EImage1 ) ;
                }
            
            observer->Commit( transactionId );
            }
     
        // Relese memory of the published text
        iDataArray.ResetAndDestroy();
        // Release memory of the published icons
        iIconArray.Reset();
        }
    
    iRfs.Close();
    }

// ---------------------------------------------------------------------------
// Cleans a data from the widget
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::Clean(MAiContentObserver* aObserver, 
        TInt aContentId )
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
void CWrtDataPlugin::Stop( TAiTransitionReason aReason )
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
void CWrtDataPlugin::Resume( TAiTransitionReason aReason )
    {
    TRAP_IGNORE( DoResumeL( aReason ) ); 
    }
    
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is not allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::Suspend( TAiTransitionReason aReason )
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
void CWrtDataPlugin::SubscribeL( MAiContentObserver& aObserver )
    { 
    iObservers.AppendL( &aObserver );
    }
 
// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-ins take ownership of the settings array, so it must either
// store it in a member or free it.
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    if( iDataCount > 0 )
        {
        // We own the array so destroy it
         aSettings.ResetAndDestroy();
         return;
        }
    
    RAiSettingsItemArray contentItemsArr;
    RAiSettingsItemArray configurationItemsArr;
    RAiSettingsItemArray settingItemsArr;
    
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
        else 
            {
            settingItemsArr.Append(pluginSetting);
            }
        }
    
    iDataCount = contentItemsArr.Count();
    if(iDataCount > 0 )
        {
        // Create the content Model
        HBufC16* contentId = HBufC16::NewLC( KAiContentIdMaxLength + KAiPluginNameMaxLength );
        iContentModel = new TAiContentItem[iDataCount];
        for(TInt i = 0; i < iDataCount; i++)
            {
            MAiPluginContentItem& contentItem = (contentItemsArr[i])->AiPluginContentItem();
            iContentModel[i].id = i;
            /*if( contentItem.Type() == KText() || contentItem.Type() == KNewsTicker() )
                {
                // text
                iContentModel[i].type = KAiContentTypeText;
                }*/
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

        iPluginState = ESuspend;
        // Register for notifications
        iData->RegisterL();
        
        // Activate the publisher 
        iData->ActivateL();
        }
    
    settingItemsArr.Reset();
    contentItemsArr.Reset();
    configurationItemsArr.Reset();
       // We own the array so destroy it
    aSettings.ResetAndDestroy();
    // publish the initial data
    PublishL();
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Returns the extension interface. Actual type depends on the passed 
// aUid argument.
// ---------------------------------------------------------------------------
//
TAny* CWrtDataPlugin::Extension( TUid aUid )
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
TAny* CWrtDataPlugin::GetPropertyL( TInt aProperty )
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
void CWrtDataPlugin::SetPropertyL( TInt aProperty, TAny* aValue )
    {  
    switch ( aProperty )
        {
		case EAiPublisherInfo:
			{
			if( aValue )
			    {
    		    const TAiPublisherInfo* info = static_cast<const TAiPublisherInfo*>( aValue );
	    	    iInfo.iName.Copy( info->iName );
	    	    iInfo.iNamespace.Copy( info->iNamespace );
			    }
		    break;
		    }
		default:
		    break;         
        }
    }
 
// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::HandleEvent( TInt /*aEvent*/, const TDesC& /*aParam*/ )
	{
    // This is not as there is no event id to retrieve in this dynamic plugin. 
	} 
    
// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::HandleEvent( const TDesC& aEventName, const TDesC& aParam )
    {
    // We have no way of reporting errors to framework so just ignore them.
    TRAP_IGNORE(iData->ExecuteActionL( aEventName , aParam ) );
    }

// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Invoked by the framework for querying if plugin has menu item
// ---------------------------------------------------------------------------
//
TBool CWrtDataPlugin::HasMenuItem( const TDesC16& aMenuItem )
	{ 
	return iData->HasMenuItem ( aMenuItem );  
	}

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// framework instructs plug-in that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::DoResumeL( TAiTransitionReason aReason )
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
TBool CWrtDataPlugin::IsActive()
    {
    return (iPluginState == EResume );
    }
