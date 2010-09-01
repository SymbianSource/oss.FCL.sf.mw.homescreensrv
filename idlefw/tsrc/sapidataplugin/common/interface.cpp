/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

// Class header
#include "interface.h"

// External includes
//#include <coemain.h>
//#include <f32file.h>
//#include <gulicon.h>
//#include <aicontentmodel.h>

//const char KAiShortcutCaption[] = "ShortcutCaption";
//const char KAiShortcutIcon[] = "ShortcutIcon";

// ======== MEMBER FUNCTIONS ========

//---------------------------------------------------------------------------
// Interface_EU::Interface_EU()
//---------------------------------------------------------------------------
//
Interface_EU::Interface_EU()
    {
    }

//---------------------------------------------------------------------------
// void Interface_EU::ConstructL()
//---------------------------------------------------------------------------
//
void Interface_EU::ConstructL()
    {
    }

//---------------------------------------------------------------------------
// Interface_EU* Interface_EU::NewL()
//---------------------------------------------------------------------------
//
Interface_EU* Interface_EU::NewL()
    {
    Interface_EU* self = new( ELeave ) Interface_EU();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//---------------------------------------------------------------------------
// Interface_EU::~Interface_EU()
//---------------------------------------------------------------------------
//
Interface_EU::~Interface_EU()
    {
    }

// ---------------------------------------------------------------------------
// void Interface_EU::ExecuteCmdL(const TDesC8& aCmdName, const CLiwGenericParamList& aInParamList, CLiwGenericParamList& aOutParamList, TUint aCmdOptions = 0, MLiwNotifyCallback* aCallback = 0);
// ---------------------------------------------------------------------------
//
void Interface_EU::ExecuteCmdL(const TDesC8& /*aCmdName*/,
                            const CLiwGenericParamList& /*aInParamList*/,
							CLiwGenericParamList& /*aOutParamList*/,
							TUint /*aCmdOptions*/,
							MLiwNotifyCallback* /*aCallback*/)
    {
    }
    
void Interface_EU::Close()
	{
	}

// End of File.
