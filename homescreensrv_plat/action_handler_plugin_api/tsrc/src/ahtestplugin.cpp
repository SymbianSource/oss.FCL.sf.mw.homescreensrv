/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Exemplary Action Handler Plugin
*
*/



#include <e32def.h>
#include <e32uid.h>
#include <LiwVariant.h>

#include "ahtestplugin.h"

_LIT8( KTitle , "title" );
_LIT8( KMessage, "message" );

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CActionHandlerTestPlugin::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CActionHandlerTestPlugin* CActionHandlerTestPlugin::NewL()
    {
    CActionHandlerTestPlugin* self = CActionHandlerTestPlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CActionHandlerTestPlugin* CActionHandlerTestPlugin::NewLC()
    {
    CActionHandlerTestPlugin* self = new( ELeave ) CActionHandlerTestPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CActionHandlerTestPlugin::~CActionHandlerTestPlugin()
	{        
	}

// ---------------------------------------------------------------------------
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CActionHandlerTestPlugin::ExecuteActionL( const CLiwMap* aMap )
    {
    TInt errCode( KErrNotFound ); 
    TLiwVariant variant; 
    variant.PushL(); 
    errCode = ExtractVariantL( aMap, variant, KTitle );
    if( errCode == KErrNone )
        {
        errCode = ExtractVariantL( aMap, variant, KMessage );
        }
    CleanupStack::PopAndDestroy( &variant );    
    return errCode;
    }


// ---------------------------------------------------------------------------
// Extract variant
// ---------------------------------------------------------------------------
//
TInt CActionHandlerTestPlugin::ExtractVariantL( const CLiwMap* aMap,
								TLiwVariant& aVariant,
								const TDesC8& aMapName )
   
    {
    TInt errCode( KErrNone );
	
    if ( !aMap->FindL( aMapName, aVariant ) )
       	{
       	errCode = KErrNotFound;
    	}
	
	return errCode;
    }

// End of file   
