/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom/ecom.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "cpluginvalidator.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C CPluginValidator* CPluginValidator::NewL( TUid aUid,
                                                   TAny *aParameter )
    {
    CPluginValidator* self = CPluginValidator::NewLC( aUid , aParameter );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C CPluginValidator* CPluginValidator::NewLC( TUid aUid,
                                                    TAny *aParameter )
    {
    CPluginValidator* self = new( ELeave ) CPluginValidator( aUid,
                                                             aParameter );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C TAny* CPluginValidator::GetImplementation( TUid aUid )
    {
    TPluginInfo pluginInfo;
    pluginInfo.iImplementationUid = aUid;
    TInt index = iPluginArray.Find( pluginInfo,
          TIdentityRelation< TPluginInfo >(UidMatch) );
    return GetImplementation( index );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C TAny* CPluginValidator::GetImplementation( TInt aIndex )
    {
    TAny* ret( NULL );
    if (( aIndex != KErrNotFound ) && ( aIndex < iPluginArray.Count() ))
      {
      ret= iPluginArray[aIndex].iPlugin;
      }
    return ret;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CPluginValidator::GetCount( )
    {
    return iPluginArray.Count();
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CPluginValidator::~CPluginValidator()
    {
    Cancel();
    CleanPluginsTable();
    iPluginArray.Close( );
    if( iSession )
        {
        iSession->Close();
        }
    REComSession::FinalClose( );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::RunL()
    {
    iSession->NotifyOnChange( iStatus );
    SetActive();
    ManagePluginsL();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::DoCancel()
    {
    iSession->CancelNotifyOnChange( iStatus );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TInt CPluginValidator::RunError( TInt /*aError*/)
    {
    return KErrNone;
    }
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CPluginValidator::CPluginValidator( TUid aUid, TAny* aParameter ):
                                                  CActive(EPriorityStandard),
                                                  iUid( aUid ),
                                                  iParameter( aParameter )
    {

    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::ConstructL()
    {
    iSession = &REComSession::OpenL();
    ManagePluginsL();
    iSession->NotifyOnChange( iStatus );
    CActiveScheduler::Add( this );
    SetActive();
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CPluginValidator::PresentInArrayL( TPluginInfo aPluginInfo,
        const RImplInfoPtrArray& aInfoArray )
    {
    TBool result( EFalse );
    for( TInt i = 0; i < aInfoArray.Count(); i++ )
        {
        if( aPluginInfo.iImplementationUid.iUid
                == aInfoArray[i]->ImplementationUid().iUid )
            {
            result = ETrue;
            break;
            }
        }
    return result;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::DestroyPlugin( TInt aIndex )
    {
    CBase* plugin = static_cast<CBase*>( iPluginArray[aIndex].iPlugin );
    delete plugin;
    REComSession::DestroyedImplementation(
            iPluginArray[aIndex].iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::CleanPluginsTable()
    {
    for( TInt i = 0; i < iPluginArray.Count(); i++ )
        {
        DestroyPlugin( i );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::ManagePluginsL()
    {
    // Read info about all implementations into infoArray
    RImplInfoPtrArray infoArray;
    CleanupResetAndDestroyPushL( infoArray );

    REComSession::ListImplementationsL( iUid , infoArray );
    TPluginInfo pluginInfo;
    //Load new plugins
    for ( TInt i = 0; i < infoArray.Count( ); i++ )
        {
        pluginInfo.iImplementationUid = infoArray[i]->ImplementationUid( );
        pluginInfo.iVersion = infoArray[i]->Version();

        TInt index = iPluginArray.Find( pluginInfo,
              TIdentityRelation< TPluginInfo >(UidMatch) );

        if( index == KErrNotFound )
            {
            //plugin wasn't present, we load it now
            LoadPluginL( pluginInfo );
            }
        else if( pluginInfo.iVersion > iPluginArray[index].iVersion )
            {
            //plugin was present but it is a newer version,
            //remove old and load a new one
            DestroyPlugin( index );
            iPluginArray.Remove( index );
            LoadPluginL( pluginInfo );
            }
        }
    //Remove old plugins
    for( TInt i = iPluginArray.Count() - 1; i >= 0; i-- )
        {
        pluginInfo = iPluginArray[i];
        if ( !PresentInArrayL( pluginInfo, infoArray ) )
            {
            DestroyPlugin( i );
            iPluginArray.Remove( i );
            }
        }
        CleanupStack::PopAndDestroy( &infoArray );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CPluginValidator::LoadPluginL( TPluginInfo& aPluginInfo )
    {
    TAny* plug( NULL );
    TInt err( KErrNone );
    TRAP( err, plug = REComSession::CreateImplementationL(
                    aPluginInfo.iImplementationUid,
                    aPluginInfo.iDtor_ID_Key, iParameter ) );
    if( err == KErrNone && plug )
        {
        CleanupStack::PushL( plug );
        aPluginInfo.iPlugin = plug;
        iPluginArray.AppendL( aPluginInfo );
        CleanupStack::Pop( plug );
        }
    }
