/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shortcut plug-in extension data model
*
*/


#include "caiscutextdatamodel.h"
#include "caiscutextdata.h"

// ======== LOCAL DEFINITIONS ========

namespace
    {
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtDataModel* CAiScutExtDataModel::NewL()
    {
    CAiScutExtDataModel* self = CAiScutExtDataModel::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CAiScutExtDataModel* CAiScutExtDataModel::NewLC()
    {
    CAiScutExtDataModel* self = new( ELeave ) CAiScutExtDataModel;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtDataModel::~CAiScutExtDataModel()
    {
    iAiScutExtDataArray.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtDataModel::CAiScutExtDataModel()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtDataModel::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtData* CAiScutExtDataModel::SetPopupLineArrayL( 
    const TDesC& aDefinition, CDesCArray* aLineArray )
    {
    CAiScutExtData* extData = NULL;
    TInt index = FindAiScutExtData( aDefinition );
    if( index != KErrNotFound )
        {
        extData = iAiScutExtDataArray[ index ];
        }
    else
        {
        extData = CreateAiScutExtDataL( aDefinition );
        }

    extData->SwitchPopupLineArray( aLineArray );    //Takes ownership
    return extData;
    }

CAiScutExtData* CAiScutExtDataModel::ResetPopupLineArray( const TDesC& aDefinition )
    {
    CAiScutExtData* extData = NULL;
    TInt index = FindAiScutExtData( aDefinition );
    if( index != KErrNotFound )
        {
        extData = iAiScutExtDataArray[ index ];
        if( extData->Icon() )
            {
            extData->ResetPopupLineArray();
            }
        else
            {            
            iAiScutExtDataArray.Remove( index );
            delete extData;
            extData = NULL;
            }        
        }
    return extData;
    }

CAiScutExtData* CAiScutExtDataModel::SetIconL( const TDesC& aDefinition, CGulIcon* aIcon )
    {
    CAiScutExtData* extData = NULL;
    TInt index = FindAiScutExtData( aDefinition );
    if( index != KErrNotFound )
        {
        extData = iAiScutExtDataArray[ index ];
        }
    else
        {
        extData = CreateAiScutExtDataL( aDefinition );
        }

    extData->SwitchIcon( aIcon );    //Takes ownership
    return extData;
    }

CAiScutExtData* CAiScutExtDataModel::ResetIcon( const TDesC& aDefinition )
    {
    CAiScutExtData* extData = NULL;
    TInt index = FindAiScutExtData( aDefinition );
    if( index != KErrNotFound )
        {
        extData = iAiScutExtDataArray[ index ];
        if( extData->PopupLineArray() )
            {
            extData->ResetIcon();
            }
        else
            {            
            iAiScutExtDataArray.Remove( index );
            delete extData;
            extData = NULL;
            }        
        }
    return extData;
    }

TArray<CAiScutExtData*> CAiScutExtDataModel::AiScutExtDataArray() const
    {
    return iAiScutExtDataArray.Array();
    }

void CAiScutExtDataModel::RemoveAiScutExtData( const TDesC& aDefinition )
    {
    TInt index = FindAiScutExtData( aDefinition );    
    if( index != KErrNotFound )
        {
        CAiScutExtData* extData = iAiScutExtDataArray[ index ];
        iAiScutExtDataArray.Remove( index );
        delete extData;
        }
    }

TInt CAiScutExtDataModel::FindAiScutExtData( const TDesC& aDefinition ) const
    {
    const TInt count = iAiScutExtDataArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iAiScutExtDataArray[ i ]->TargetDefinition().CompareF( aDefinition ) == 0 )
            {
            return i;
            }
        }
    return KErrNotFound;
    }
    
CAiScutExtData* CAiScutExtDataModel::CreateAiScutExtDataL( const TDesC& aDefinition )
    {    
    CAiScutExtData* extData = CAiScutExtData::NewLC( aDefinition );
    iAiScutExtDataArray.AppendL( extData ); //Takes ownership
    CleanupStack::Pop( extData );
    return extData;
    }
// End of File.
