/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for accessing definitionengine
*
*/

  
// INCLUDE FILES
#include    "hspsdefinitionengineinterface.h"
#include    "hspsdefinitionengine.rh"
#include    <ecom/ecom.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDefinitionEngineInterface::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDefinitionEngineInterface* ChspsDefinitionEngineInterface::NewL( 
    const TDesC8& aPluginId )
	{
    TUid interfaceUid = {KhspsDefinitionEngineIId};
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aPluginId );
    resolverParams.SetWildcardMatch( ETrue );
	
    TAny * ecom = NULL;

#ifdef _DEBUG
	TRAPD(
        err, 
        ecom = REComSession::CreateImplementationL( 
	                interfaceUid, 
	                _FOFF(ChspsDefinitionEngineInterface, iDtorKey),
	                NULL, 
	                resolverParams )
	            );

	if( err != KErrNone )
	    {
	    REComSession::FinalClose();
	    User::Leave( err );
	    }
#else

    ecom = REComSession::CreateImplementationL( 
        interfaceUid, 
        _FOFF(ChspsDefinitionEngineInterface, iDtorKey),
	    NULL, 
	    resolverParams );
#endif
 
    ChspsDefinitionEngineInterface* element = 
        reinterpret_cast<ChspsDefinitionEngineInterface*>(ecom);	    
   
    return element;

	}
	
// -----------------------------------------------------------------------------
// ChspsDefinitionEngineInterface::Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDefinitionEngineInterface::~ChspsDefinitionEngineInterface()
	{
	REComSession::DestroyedImplementation(iDtorKey);
	}	
	
//  End of File  
