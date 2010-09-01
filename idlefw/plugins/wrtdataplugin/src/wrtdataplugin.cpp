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
* Description: WRT data plug-in publisher
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

#include "wrtdatapluginconst.h"
#include "wrtdatapluginuids.hrh"
#include "wrtdataplugin.h"
#include "wrtdata.h"

// Constants
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidDataPlugin, CWrtDataPlugin::NewL ) 
    };

const TInt KTryAgainDelay( 3000000 ); // 3 sec

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// ImplementationGroupProxy
//
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) /
        sizeof( TImplementationProxy );
    return KImplementationTable;
    }

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CWrtDataPlugin::NewL()
//
// ----------------------------------------------------------------------------
//
CWrtDataPlugin* CWrtDataPlugin::NewL()
    {
    CWrtDataPlugin* self = new (ELeave) CWrtDataPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CWrtDataPlugin::CWrtDataPlugin()
//
// ----------------------------------------------------------------------------
//
CWrtDataPlugin::CWrtDataPlugin()
    : iNetworkStatus( EUnknown ), iPluginState( EStopped ) 
    {
    }
    
// ----------------------------------------------------------------------------
// CWrtDataPlugin::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::ConstructL()
    {
    User::LeaveIfError( iRfs.Connect() );
    
    iData = CWrtData::NewL( this );
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CWrtDataPlugin::~CWrtDataPlugin()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        }

    delete iData;
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
    
    iRfs.Close();
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::Start
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::Start( TStartReason aReason )
    {
    iPluginState = EStarted;
    
    if( aReason == ESystemStartup || 
        aReason == EPluginStartup )
        {
        // Publish the initial data
        TRAP_IGNORE( PublishInitialDataL() );
        }
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::Stop
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::Stop( TStopReason aReason )
    {
    iPluginState = EStopped;
    
    if( aReason == EPluginShutdown ||
        aReason == ESystemShutdown )
        {
        TRAP_IGNORE(iData->NotifyPublisherL( KDeActive ));
        }
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::Resume
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::Resume( TResumeReason aReason )
    {
    if ( aReason == EForeground && iPluginState != EStopped )
        {
        iPluginState = EResume;

        TRAP_IGNORE( iData->NotifyPublisherL( KResume ));        
        }    
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::Suspend
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::Suspend( TSuspendReason aReason )
    {    
    if ( aReason == EBackground && iPluginState != EStopped )
        {
        iPluginState = ESuspend;
        
        TRAP_IGNORE ( iData->NotifyPublisherL( KSuspend ));        
        }        
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::SetOnline
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::SetOnline()
    {    
    if ( iPluginState != EStopped )
        {
        iNetworkStatus = EOnline;
        TRAP_IGNORE( iData->NotifyPublisherL( KOnLine ));                
        }
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::SetOffline
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::SetOffline()
    {
    if ( iPluginState != EStopped )
        {   
        iNetworkStatus = EOffline;
        TRAP_IGNORE( iData->NotifyPublisherL( KOffLine ));
        }
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::SubscribeL
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::SubscribeL( MAiContentObserver& aObserver )
    {
    iObservers.AppendL( &aObserver );
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::ConfigureL
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    RAiSettingsItemArray contentItemsArr;
    RAiSettingsItemArray configurationItemsArr;
    TInt count( aSettings.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        MAiPluginSettings* setting( aSettings[i] );
        
        if( setting->AiPluginItemType() == EAiPluginContentItem )
            {
            contentItemsArr.Append( setting );
            }
        else if( setting->AiPluginItemType() == EAiPluginConfigurationItem )
            {
            configurationItemsArr.Append( setting );
            }
        }
    
    iDataCount = contentItemsArr.Count();
 
    // Create the content Model
    HBufC16* contentId = HBufC16::NewLC( 
        KAiContentIdMaxLength + KAiPluginNameMaxLength );
    
    iContentModel = new TAiContentItem[iDataCount];
    
    for( TInt i = 0; i < iDataCount; i++ )
        {
        MAiPluginContentItem& contentItem( 
            contentItemsArr[i]->AiPluginContentItem() );
        
        iContentModel[i].id = i;
        if( contentItem.Type() == KText() )
            {
            // text
            iContentModel[i].type = KAiContentTypeText;
            }
        if( contentItem.Type() == KImage() || 
            contentItem.Type() == KAnimation() )
            {
            // image
            iContentModel[i].type = KAiContentTypeBitmap;
            }
        
        contentId->Des().Copy( contentItem.Name() );
        contentId->Des().Delete( 0, 
            contentId->Des().LocateReverse( KPluginNameSeprator ) + 1 );

        TInt sizeOfContentId( contentId->Des().Size()+sizeof( wchar_t ) );
        
        iContentModel[i].cid = 
            static_cast< const wchar_t* >( User::Alloc( sizeOfContentId ) );
            
        Mem::Copy( ( TAny* )iContentModel[i].cid, 
            contentId->Des().PtrZ(), sizeOfContentId );
        
        contentId->Des().Delete( 0, contentId->Des().Length() );
        }    
    
    CleanupStack::PopAndDestroy( contentId );
    iContent = AiUtility::CreateContentItemArrayIteratorL( 
            iContentModel, iDataCount );
                   
    // Configurations 
    iData->ConfigureL( configurationItemsArr );

    // Register for notifications
    iData->RegisterL();
    
    // Activate the publisher 
    iData->NotifyPublisherL( KActive );
    
    contentItemsArr.Reset();
    configurationItemsArr.Reset();
    // We own the array so destroy it
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::SetProperty
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::SetProperty( TProperty aProperty, TAny* aAny )
    {
    if (aProperty == ECpsCmdBuffer )
        {   
        iData->SetCommandBuffer( aAny, PublisherInfo().Namespace() );
        }
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::GetProperty
//
// ----------------------------------------------------------------------------
//
TAny* CWrtDataPlugin::GetProperty( TProperty aProperty )
    {
    if ( aProperty == EPublisherContent )
        {
        return static_cast< MAiContentItemIterator* >( iContent );      
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::HandleEvent
//
// ----------------------------------------------------------------------------
//
void CWrtDataPlugin::HandleEvent( const TDesC& aEventName, 
    const TDesC& aParam )
    {
    TRAP_IGNORE( iData->ExecuteActionL( aEventName , aParam ) );    
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::IsActive
//
// ----------------------------------------------------------------------------
//
TBool CWrtDataPlugin::IsActive() const
    {
    return iPluginState == EResume;
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::IsStopped
//
// ----------------------------------------------------------------------------
//
TBool CWrtDataPlugin::IsStopped() const
    {
    return iPluginState == EStopped;
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::Data
//
// ----------------------------------------------------------------------------
//
CWrtData* CWrtDataPlugin::Data() const
    {
    return iData;
    }

// ----------------------------------------------------------------------------
// CWrtDataPlugin::NetworkStatus
//
// ----------------------------------------------------------------------------
//
CWrtDataPlugin::TPluginNetworkStatus CWrtDataPlugin::NetworkStatus() const
    {
    return iNetworkStatus;
    }

// ---------------------------------------------------------------------------
// CWrtDataPlugin::GetIdL
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
// CWrtDataPlugin::GetTypeL
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
//Refresh a specific image or text in the widget
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::RefreshL( TDesC16& aOperation, CLiwDefaultMap* aDataMap )
    {
    __PRINTS("*** CWrtDataPlugin::RefreshL ***");
    
    __PRINT( __DBG_FORMAT( "* Publisher name: %S, uid: 0x%x, operation: %S" ),          
        &PublisherInfo().Name(), PublisherInfo().Uid().iUid, &aOperation ); 
    
    TInt observers( iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( this );
    
    for ( TInt obsIndex = 0; obsIndex < observers; obsIndex++ )
        {
        MAiContentObserver* observer = iObservers[obsIndex];
 
        if ( observer->StartTransaction( transactionId ) == KErrNone )       
            {
            if( ( aOperation == KOperationUpdate 
                  || aOperation == KOperationAdd )
                  && aDataMap )
                {
                iData->PublishL( observer, aDataMap );
                }
            else if ( aOperation == KOperationDelete )
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
    
    __PRINTS("*** CWrtDataPlugin::RefreshL - done ***");        
    }

// ---------------------------------------------------------------------------
// Publish a specific text of the widget  
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::PublishTextL(MAiContentObserver* aObserver, 
        TInt aContentId, const TDesC16& aContentValue)
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
    }

// ---------------------------------------------------------------------------
// Publish a specific image of the widget  
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::PublishImageL(MAiContentObserver* aObserver,
		TContentItem aContentId, const TDesC16& aPath )
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
      TBool inSkin = ResolveSkinIdAndMifId( aPath, iconId, bitmapId, maskId, fileName  );
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
// Show the loading icong animation 
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::ShowLoadingIcon(MAiContentObserver* aObserver)
    {
    aObserver->SetProperty( *this, KElement , KDisplay , KShow );
    }

// ---------------------------------------------------------------------------
// Hides the loading icon animation 
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::HideLoadingIcon(MAiContentObserver* aObserver)
    {
    aObserver->SetProperty( *this, KElement , KDisplay , KHide );

    // Do not try to publish initial data anymore
    StopTimer();
    }

// ---------------------------------------------------------------------------
// Publishes widget's texts and images
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::PublishInitialDataL()
    {
    TInt observers( iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( this );

    for ( int i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer = iObservers[i];

        CleanupStack::PushL( TCleanupItem( CancelTransaction, observer ) );

        if ( observer->StartTransaction( transactionId ) == KErrNone )           
            {// Publish default data
            iData->PublishInitialDataL(observer);
            observer->Commit( transactionId );
            }

		CleanupStack::Pop( observer );

        // Release memory of the published text
        iDataArray.ResetAndDestroy();
        // Release memory of the published icons
        iIconArray.Reset();
        }

    }

// ---------------------------------------------------------------------------
// ResolveSkinItemId
// ---------------------------------------------------------------------------
//
TBool CWrtDataPlugin::ResolveSkinIdAndMifId( const TDesC& aPath, TAknsItemID& aItemId,
        TInt& abitmapId, TInt& aMaskId, TDes& aFilename )
   {
   // Syntax: skin( <major> <minor> ):mif(filename bimapId maskId) 
   TInt error = KErrNotFound;
   TInt pos = aPath.FindF( KSkin );
   if( pos != KErrNotFound )
       {
       // Skip skin token
       pos += KSkin().Length();
       
       // Initialize lexer
      TLex lex( aPath.Mid( pos ) );
      lex.SkipSpace();
       
       // Check left parenthesis
      if (lex.Get() == KLeftParenthesis )
           {
           //lex.SkipSpace();
           
           TInt majorId( 0 );        
           TInt minorId( 0 );

           // Resolve major id        
           error = lex.Val( majorId );
           
           // Resolve minor id
           lex.SkipSpace();
           error |= lex.Val( minorId );
           
           // initilize skin item id object
           aItemId.Set( majorId, minorId );
           }
       }

   if( (error == KErrNone && aPath.FindF( KColon ) != KErrNotFound ) 
         || ( error == KErrNotFound ) )
       {
       error = KErrNotFound;
       pos = aPath.FindF( KMif );
       if ( pos != KErrNotFound )
           {
           pos += KMif().Length();
           // Initialize lexer
           TLex lex( aPath.Mid( pos ) );
           lex.SkipSpace();
           
           // Check left parenthesis
           if (lex.Get() == KLeftParenthesis )
               {
               lex.SkipSpaceAndMark();
               lex.SkipCharacters();
               // Resolve MifFile name
               aFilename.Copy(lex.MarkedToken());
               if( aFilename.Length()!= 0)
                   {
                   // Resolve bitmap id  
                   lex.SkipSpace();
                   error = lex.Val( abitmapId );
                   
                   // Resolve mask id
                   // dont return error if it is not found, that is ok
                   lex.SkipSpace();
                   lex.Val( aMaskId );
                   }
               else
                   {
                   error = KErrNotFound;
                   }
               }
           }
       }
   return (error == KErrNone );
   }

// ---------------------------------------------------------------------------
// Cleanup callback for cancelling a transactions in case of leave
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::CancelTransaction( TAny* aObserver )
    {
    if ( aObserver )
        {
        MAiContentObserver* obs = reinterpret_cast< MAiContentObserver*>( aObserver );
        TInt transactionId = reinterpret_cast<TInt>( aObserver );
        obs->CancelTransaction( transactionId );
        }
    }

// ---------------------------------------------------------------------------
// Create and start republish timer
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::StartTimer()
    {
    TRAP_IGNORE(
        if ( !iTimer )
            {
            iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
            }
        
        if ( iTimer && !iTimer->IsActive() )
            {
            TTimeIntervalMicroSeconds32 delay( KTryAgainDelay );
            iTimer->Start( delay, delay, TCallBack( Timeout, this ) );
            }
        );
    }

// ---------------------------------------------------------------------------
// Cancel republish timer
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::CancelTimer()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// Stop and delete republish timer
// ---------------------------------------------------------------------------
//
void CWrtDataPlugin::StopTimer()
    {
    if ( iTimer )
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Initial data republish callback
// ---------------------------------------------------------------------------
//
TInt CWrtDataPlugin::Timeout( TAny* aPtr )
    {
    CWrtDataPlugin* self = static_cast<CWrtDataPlugin*>( aPtr );

    // Cancel timer before publishing
    self->CancelTimer();

    TInt observers( self->iObservers.Count() );        
    TInt transactionId = reinterpret_cast<TInt>( self );
    TBool success( ETrue );

    // Publish for each observer
    for ( int i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer = self->iObservers[i];

        if ( observer->StartTransaction( transactionId ) == KErrNone )           
            {
            // Publish default image
            TRAPD( err, self->iData->PublishDefaultImageL( observer ) );
            if ( KErrNone != err )
                {
                observer->CancelTransaction( transactionId );
                success = EFalse;
                }
            else
                {
                // 
                observer->Commit( transactionId );
                }
            }
        }

    // Start timer again if there is error in publishing
    if ( !success )
        {
        self->StartTimer();
        }
    else
        {
        self->StopTimer();
        }

    // Release memory of the published icons
    self->iIconArray.Reset();

    return KErrNone;
    }



// End of file
