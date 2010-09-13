/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description :
 *
 */
#include <bitmaptransforms.h>
#include <imageconversion.h>

#include "tsgraphicfilescalinghandler.h"
// -----------------------------------------------------------------------------
/**
 * Private constructor.
 * Parameters - the same meaning as in appropriate NewL/NewLC functions.
 */
CTsGraphicFileScalingHandler::CTsGraphicFileScalingHandler(
                                MImageReadyCallBack &aNotify,
                                const TSize &aNewSize,
                                TKindOfScaling aKindOfScaling,
                                TInt aRotation )
:
    CActive( EPriorityNormal ),
    iNotify( aNotify ),
    iNewSize( aNewSize ),
    iKindOfScaling( aKindOfScaling ),
    iRotation( aRotation ),
    iCurrentOperation( ENone )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
/**
* Destructor.
*/
CTsGraphicFileScalingHandler::~CTsGraphicFileScalingHandler()
    {
    Cancel();
    delete iInputBitmap;
    delete iOutputBitmap;
    delete iImageDecoder;
    delete iBitmapScaler;
    delete iBitmapRotator;
    }

// -----------------------------------------------------------------------------
/**
 * Constructors for activation graphic file scaling.
 * aNotify        - reference to observer implementation.
 * aFs            - reference to file server session.
 * aFileName      - path to graphic file.
 * aMimeType      - mime type of graphic file.
 * aNewSize       - new size of output graphic file.
 * aKindOfScaling - kind of graphic file scaling described above.
 * aRotation - requested rotation angle
 */
CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewL(
                                                MImageReadyCallBack &aNotify,
                                                RFs &aFs,
                                                const TDesC &aFileName,
                                                const TDesC8& aMimeType,
                                                const TSize &aNewSize,
                                                TKindOfScaling aKindOfScaling,
                                                TInt aRotation
                                                )
    {
    CTsGraphicFileScalingHandler *self = 
                            CTsGraphicFileScalingHandler::NewLC(aNotify,
                                                                aFs,
                                                                aFileName,
                                                                aMimeType,
                                                                aNewSize,
                                                                aKindOfScaling,
                                                                aRotation );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
/**
 * Constructors for activation graphic file scaling.
 * aNotify        - reference to observer implementation.
 * aFs            - reference to file server session.
 * aFileName      - path to graphic file.
 * aMimeType      - mime type of graphic file.
 * aNewSize       - new size of output graphic file.
 * aKindOfScaling - kind of graphic file scaling described above.
 * aRotation - requested rotation angle
 */
CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewLC(
                                                MImageReadyCallBack &aNotify,
                                                RFs &aFs,
                                                const TDesC &aFileName,
                                                const TDesC8& aMimeType,
                                                const TSize &aNewSize,
                                                TKindOfScaling aKindOfScaling,
                                                TInt aRotation )
    {
    CTsGraphicFileScalingHandler *self = 
                    new (ELeave) CTsGraphicFileScalingHandler( aNotify,
                                                               aNewSize,
                                                               aKindOfScaling,
                                                               aRotation );

    CleanupStack::PushL( self );
    self->ConstructL( aFs, aFileName, aMimeType );
    return self;
    }

// -----------------------------------------------------------------------------
/**
 * Functions construct active objest instance and made asynchronous operation/s.
 * Parameters - the same meaning as in appropriate NewL/NewLC functions.
 */
void CTsGraphicFileScalingHandler::ConstructL( RFs &aFs, 
                                               const TDesC &aFileName, 
                                               const TDesC8& aMimeType )
    {
    if( 0 == aFileName.Length()
         || EFalse == aFs.IsValidName( aFileName ) )
        {
        User::Leave( KErrPathNotFound );
        }

    if( 0 == aMimeType.Length() )
        {
        User::Leave( KErrBadName );
        }

    if( 0 >= iNewSize.iWidth || 0 >= iNewSize.iHeight )
        {
        User::Leave(KErrCorrupt);
        }
    iInputBitmap = new(ELeave)CFbsBitmap();
    DecodingOperationL( aFs, aFileName, aMimeType );
    SetActive();
    }

// -----------------------------------------------------------------------------
/**
 * Exported from dll constructors for activation graphic file scaling.
 * aNotify         - reference to observer implementation.
 * aInputFbsBitmap - reference to pattern CFbsBitmap.
 * aNewSize        - new size of output graphic file.
 * aKindOfScaling  - kind of graphic file scaling described above.
 * aRotation - requested rotation angle
 */
CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewL(
                                        MImageReadyCallBack &aNotify,
                                        const CFbsBitmap &aInputFbsBitmap,
                                        const TSize &aNewSize,
                                        TKindOfScaling aKindOfScaling,
                                        TInt aRotation)

{
    CTsGraphicFileScalingHandler *self = 
                            CTsGraphicFileScalingHandler::NewLC( aNotify,
                                                                 aInputFbsBitmap,
                                                                 aNewSize,
                                                                 aKindOfScaling,
                                                                aRotation );
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
/**
 * Exported from dll constructors for activation graphic file scaling.
 * aNotify         - reference to observer implementation.
 * aInputFbsBitmap - reference to pattern CFbsBitmap.
 * aNewSize        - new size of output graphic file.
 * aKindOfScaling  - kind of graphic file scaling described above.
 * aRotation - requested rotation angle
 */
CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewLC(
                                            MImageReadyCallBack &aNotify,
                                            const CFbsBitmap &aInputFbsBitmap,
                                            const TSize &aNewSize,
                                            TKindOfScaling aKindOfScaling,
                                            TInt aRotation)
    {
    CTsGraphicFileScalingHandler *self = 
                            new (ELeave) CTsGraphicFileScalingHandler(
                                                               aNotify,
                                                               aNewSize,
                                                               aKindOfScaling,
                                                               aRotation );
    CleanupStack::PushL( self );
    self->ConstructL( aInputFbsBitmap );
    return self;
    }

// -----------------------------------------------------------------------------
/**
 * Functions construct active objest instance and made asynchronous operation/s.
 * Parameters - the same meaning as in appropriate NewL/NewLC functions.
 */
void CTsGraphicFileScalingHandler::ConstructL( const CFbsBitmap &aInputFbsBitmap )
    {
    if( 0 >= iNewSize.iWidth || 0 >= iNewSize.iHeight)
        {
        User::Leave( KErrCorrupt );
        }

    iInputBitmap = new(ELeave)CFbsBitmap();
    User::LeaveIfError( iInputBitmap->Duplicate( aInputFbsBitmap.Handle() ) );

    IsSupportedRotationMode() ? RotationOperationL() : ScalingOperationL();
    SetActive();
    }

// -----------------------------------------------------------------------------
/**
 * Cancels the wait for completion of an outstanding request.
 */
void CTsGraphicFileScalingHandler::DoCancel()
    {
    switch(iCurrentOperation)
        {
    case EConvertBitmapFromFile:
        iImageDecoder->Cancel();
        break;
    case EScale:
        iBitmapScaler->Cancel();
        break;
        }
    iNotify.ImageReadyCallBack( KErrCancel, 0 );
    }

// -----------------------------------------------------------------------------
/**
 * Handles an active object’s request completion event.
 */
void CTsGraphicFileScalingHandler::RunL()
    {
    User::LeaveIfError(iStatus.Int());

    switch (iCurrentOperation)
        {
    case EConvertBitmapFromFile:
            delete iImageDecoder;
            iImageDecoder = 0;
            
            IsSupportedRotationMode() ? RotationOperationL() : ScalingOperationL();
            SetActive();
            break;
    
    case ERotate:
            delete iBitmapRotator;
            iBitmapRotator = 0;
            
            ScalingOperationL();
            SetActive();
            break;
    
    case EScale:
            iCurrentOperation = ENone;
            delete iBitmapScaler;
            iBitmapScaler = 0;
            delete iInputBitmap;
            iInputBitmap = 0;
            if (iKindOfScaling == CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding) 
                {
                User::LeaveIfError(iOutputBitmap->Resize(iNewSize));
                }
            iNotify.ImageReadyCallBack(iStatus.Int(), iOutputBitmap);
            break;
        
        }
    }

// -----------------------------------------------------------------------------
/**
 * Action to made before decoding graphic file operation.
 * Parameters - the same meaning as in appropriate NewL/NewLC functions.
 */
void CTsGraphicFileScalingHandler::DecodingOperationL( RFs &aFs, 
                                                       const TDesC &aFileName, 
                                                       const TDesC8& aMimeType )
    {
    // convert *.png to bitmap
    iImageDecoder = CImageDecoder::FileNewL( aFs, aFileName, aMimeType );
    const TFrameInfo frameInfo( iImageDecoder->FrameInfo( 0 ) );
    iInputBitmap->Reset();
    User::LeaveIfError( iInputBitmap->Create(frameInfo.iOverallSizeInPixels, 
                                             frameInfo.iFrameDisplayMode ) );
    iImageDecoder->Convert( &iStatus, *iInputBitmap, 0 );
    iCurrentOperation = EConvertBitmapFromFile;
    }

// -----------------------------------------------------------------------------
/**
 * Action to made before scaling graphic file operation.
 */
void CTsGraphicFileScalingHandler::ScalingOperationL()
    {
    iBitmapScaler = CBitmapScaler::NewL();
    iBitmapScaler->SetQualityAlgorithm( CBitmapScaler::EMaximumQuality );
    FixForDisplayModeNotSupportedByScalingOperation();
    iOutputBitmap = new (ELeave)CFbsBitmap();
    User::LeaveIfError(iOutputBitmap->Create( NewSizeToScalingOperation(), 
                                              iInputBitmap->DisplayMode() ) );
    iBitmapScaler->Scale( &iStatus, *iInputBitmap, *iOutputBitmap, EFalse );
    iCurrentOperation = EScale;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::RotationOperationL()
    {
    const CBitmapRotator::TRotationAngle rotation =
        static_cast<CBitmapRotator::TRotationAngle>( RotationMode() );
    iBitmapRotator = CBitmapRotator::NewL();
    iBitmapRotator->Rotate( &iStatus, 
                            *iInputBitmap, 
                            rotation );
    iCurrentOperation = ERotate;
    }

// -----------------------------------------------------------------------------
//
TInt CTsGraphicFileScalingHandler::RotationMode()const
    {
    const TInt rotation(iRotation%360);
    TInt retVal(CBitmapRotator::EMirrorHorizontalAxis);
    if( 270 <= rotation )
        {
        retVal = CBitmapRotator::ERotation270DegreesClockwise;
        }
    else if( 180 <= rotation )
        {
        retVal = CBitmapRotator::ERotation180DegreesClockwise;
        }
    else if( 90 <= rotation )
        {
        retVal = CBitmapRotator::ERotation90DegreesClockwise;
        }
    return retVal;
    }

// -----------------------------------------------------------------------------
TBool CTsGraphicFileScalingHandler::IsSupportedRotationMode() const
    {
    TBool retVal(EFalse);
    switch( RotationMode() )
        {
        case CBitmapRotator::ERotation90DegreesClockwise:
        case CBitmapRotator::ERotation180DegreesClockwise:
        case CBitmapRotator::ERotation270DegreesClockwise:
            retVal = ETrue;
            break;
        }
    return retVal;
    }
// -----------------------------------------------------------------------------
/**
 * Fix for TDisplayMode == EColor16MAP not supported by scaling operation!
 * ! ! ! ADD OTHER NOT SUPPORTED DISPLAY MODES ! ! !
 */
void CTsGraphicFileScalingHandler::FixForDisplayModeNotSupportedByScalingOperation()
    {
    if (EColor16MAP == iInputBitmap->DisplayMode()) 
        {
        iInputBitmap->SetDisplayMode(EColor16MA);
        }
    }

// -----------------------------------------------------------------------------
/**
 * Algorithm to determine output bitmap (returned in ImageReadyCallBack) size
 * after scaling operation.
 */
TSize CTsGraphicFileScalingHandler::NewSizeToScalingOperation()
    {
    TSize originalSize = iInputBitmap->SizeInPixels();
    float widthFactor = iNewSize.iWidth / (float)originalSize.iWidth;
    float heightFactor = iNewSize.iHeight / (float)originalSize.iHeight;
    TSize retSize(iNewSize);

    if(CTsGraphicFileScalingHandler::EKeepAspectRatio == iKindOfScaling) 
       {
       retSize = (widthFactor < heightFactor) ?
                 TSize(iNewSize.iWidth, widthFactor * originalSize.iHeight) :
                 TSize(heightFactor * originalSize.iWidth, iNewSize.iHeight);
       } 
    else if (CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding == iKindOfScaling) 
       {
       retSize = (widthFactor < heightFactor) ?
                 TSize(heightFactor * originalSize.iWidth, iNewSize.iHeight) :
                 TSize(iNewSize.iWidth, widthFactor * originalSize.iHeight);
       }
    return retSize;
    }
// -----------------------------------------------------------------------------
/**
 * Handles a leave occurring in the request completion event handler RunL().
 */
TInt CTsGraphicFileScalingHandler::RunError( TInt aError )
    {
    iNotify.ImageReadyCallBack( aError, 0 );
    return KErrNone;
    }
