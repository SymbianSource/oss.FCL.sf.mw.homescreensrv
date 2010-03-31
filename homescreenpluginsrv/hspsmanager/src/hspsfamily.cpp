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
* Description:  Family gets resolution and orientation
*
*/

#include "hspsfamily.h"
#include "hsps_builds_cfg.hrh"
#include "hspsmanifest.h"
#include <featmgr.h>


_LIT8(KTch, "_tch");
const TInt KMaxFamilyLength( 20 );


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// ChspsFamily::NewL
// -----------------------------------------------------------------------------
//
ChspsFamily* ChspsFamily::NewL()
    {
    ChspsFamily* self = new(ELeave) ChspsFamily();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsFamily::ChspsFamily
// -----------------------------------------------------------------------------
//
ChspsFamily::ChspsFamily() 
    {
    }

// -----------------------------------------------------------------------------
// ChspsFamily::ConstructL
// -----------------------------------------------------------------------------
//
void ChspsFamily::ConstructL()
    {    

    User::LeaveIfError( iWsSession.Connect() );
            
    iScreenDevice = new (ELeave) CWsScreenDevice( iWsSession );

    User::LeaveIfError( iScreenDevice->Construct() );  
    
    }


// ------------------------------------------------------------------------------
// ChspsFamily::~ChspsFamily
// ------------------------------------------------------------------------------
ChspsFamily::~ChspsFamily()
    {

    delete iScreenDevice;
    iWsSession.Close();    
    }

// -----------------------------------------------------------------------------
// ChspsFamily::GetFamilyString 
// -----------------------------------------------------------------------------
void ChspsFamily::GetFamilyString(        
        TDes8& aFamily )
    {                     
    // Append input with a prefix based on the active screen resolution    
    TPixelsTwipsAndRotation sizeAndRotation;
    iScreenDevice->GetDefaultScreenSizeAndRotation( sizeAndRotation );
    TSize resolution( sizeAndRotation.iPixelSize );  
    if( resolution.iWidth > resolution.iHeight )
        {
        TInt temp = resolution.iHeight;
        resolution.iHeight = resolution.iWidth;
        resolution.iWidth = temp;        
        }          
    switch( resolution.iHeight )
        {
        case 320:
            {
            if ( resolution.iWidth == 240 )
                {
                aFamily.Append( KFamilyQvga );
                }
            }
            break;
        case 640:
            {
            if( resolution.iWidth == 360 )
                {
                aFamily.Append( KFamilyQhd );
                }
            else if( resolution.iWidth == 480 )
                {
                aFamily.Append( KFamilyVga );
                }            
            }
            break;
        
        default:
            break;
        }    
    if( aFamily.Length() > 0 )
        {                     
        aFamily.Append( KTch );
        }
    }

// -----------------------------------------------------------------------------
// ChspsFamily::GetFamilyType 
// -----------------------------------------------------------------------------
ThspsFamily ChspsFamily::GetFamilyType( 
        const TDesC8& aFamilyString )
    {
    ThspsFamily family( EhspsFamilyUnknown );
    
    if( aFamilyString == KFamilyQvga )
       {
       family = EhspsFamilyQvga;
       }
    else if( aFamilyString == KFamilyQvga2 )
       {
       family = EhspsFamilyQvga2;
       }
    else if( aFamilyString == KFamilyVga )
       {
       family = EhspsFamilyVga;            
       }
    else if( aFamilyString == KFamilyVga3 )
       {
       family = EhspsFamilyVga3;
       }
    else if( aFamilyString == KFamilyQhd )
       {
       family = EhspsFamilyQhd;
       }
    else if( aFamilyString == KFamilyQhd_tch )
       {
       family = EhspsFamilyQhd_tch;
       }
   else if( aFamilyString == KFamilyVga_tch )
       {
       family = EhspsFamilyVga_tch;
       }    
   
    return family;     
    }

// -----------------------------------------------------------------------------
// ChspsFamily::GetFamilyType
// -----------------------------------------------------------------------------
ThspsFamily ChspsFamily::GetFamilyType()
    {
    TBuf8<KMaxFamilyLength> familyString;
    GetFamilyString( familyString );   
    return GetFamilyType( familyString );
    }

 

// End of File

