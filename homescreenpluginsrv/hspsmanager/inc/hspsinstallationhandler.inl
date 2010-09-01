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
* Description:  ?Description
*
*/


// -----------------------------------------------------------------------------
// From MhspsInstallationService 
// This method is not supported
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsInstallationHandler::hspsInstallTheme( 
		const TDesC& /*aManifestFileName*/,
		ChspsODT& /*aHeader*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// From MhspsInstallationService
// This method is not supported 
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsInstallationHandler::hspsInstallNextPhaseL( 
		ChspsODT& /*aHeader*/ )
    {    
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::OnStartDocumentL(
		const RDocumentParameters& /*aDocParam*/, 
		TInt /*aErrorCode*/)
    {       
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::OnEndDocumentL(
		TInt /*aErrorCode*/)
    {    
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::OnError(
		TInt /*aErrorCode*/)
    {    
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::OnProcessingInstructionL(
	    const TDesC8& /*aTarget*/, 
	    const TDesC8& /*aData*/, 
		TInt /*aErrorCode*/)
    {      
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::OnStartPrefixMappingL(
	    const RString& /*aPrefix*/, 
	    const RString& /*aUri*/, 
	    TInt /*aErrorCode*/)
    {       
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::OnEndPrefixMappingL(
		const RString& /*aPrefix*/,
		TInt /*aErrorCode*/)
    {       
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::OnIgnorableWhiteSpaceL(
		const TDesC8& /*aBytes*/, 
		TInt /*aErrorCode*/)
    {    
    }    

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::OnSkippedEntityL(
		const RString& /*aName*/, 
		TInt /*aErrorCode*/) 
    {    
    }

// -----------------------------------------------------------------------------
// From MContentHandler
// -----------------------------------------------------------------------------
//
TAny* ChspsInstallationHandler::GetExtendedInterface(
		const TInt32 /*aUid*/)
    {   
    return NULL;    
    }    