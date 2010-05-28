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
#include "tsgraphicfilescalinghandler.h"
#include <imageconversion.h>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CTsGraphicFileScalingHandler::CTsGraphicFileScalingHandler(MImageReadyCallBack &aNotify,
                                const TSize &aNewSize,
                                TKindOfScaling aKindOfScaling
                                /* = CTsGraphicFileScalingHandler::EIgnoreAspectRatio*/) :
    CActive(EPriorityNormal),
    mNotify(aNotify),
    mNewSize(aNewSize),
    mKindOfScaling(aKindOfScaling),
    mCurrentOperation(ENone)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C CTsGraphicFileScalingHandler::~CTsGraphicFileScalingHandler()
{
    Cancel();
    delete mInputBitmap;
    delete mOutputBitmap;
    delete mImageDecoder;
    delete mBitmapScaler;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewL(MImageReadyCallBack &aNotify,
                                                RFs &aFs,
                                                const TDesC &aFileName,
                                                const TDesC8& aMimeType,
                                                const TSize &aNewSize,
                                                TKindOfScaling aKindOfScaling
                                                /* = CTsGraphicFileScalingHandler::EIgnoreAspectRatio*/)
{
    CTsGraphicFileScalingHandler *self = CTsGraphicFileScalingHandler::NewLC(aNotify,
                                                                        aFs,
                                                                        aFileName,
                                                                        aMimeType,
                                                                        aNewSize,
                                                                        aKindOfScaling);
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewLC(MImageReadyCallBack &aNotify,
                                                RFs &aFs,
                                                const TDesC &aFileName,
                                                const TDesC8& aMimeType,
                                                const TSize &aNewSize,
                                                TKindOfScaling aKindOfScaling
                                                /* = CTsGraphicFileScalingHandler::EIgnoreAspectRatio*/)
{
    CTsGraphicFileScalingHandler *self = new (ELeave) CTsGraphicFileScalingHandler(aNotify,
                                                                                aNewSize,
                                                                                aKindOfScaling);

    CleanupStack::PushL(self);
    self->ConstructL(aFs, aFileName, aMimeType);
    return self;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::ConstructL(RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType)
{
    if (aFileName.Length() == 0
        || aFs.IsValidName(aFileName) == EFalse) {
        User::Leave(KErrPathNotFound);
    }

    if (aMimeType.Length() == 0) {
        User::Leave(KErrBadName);
    }

    if (0>=mNewSize.iWidth || 0>=mNewSize.iHeight) {
       User::Leave(KErrCorrupt);
    }

    mInputBitmap = new(ELeave)CFbsBitmap();
    DecodingOperationL(aFs, aFileName, aMimeType);
    SetActive();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewL(MImageReadyCallBack &aNotify,
                                        const CFbsBitmap &aImputFbsBitmap,
                                        const TSize &aNewSize,
                                        TKindOfScaling aKindOfScaling
                                        /* = CTsGraphicFileScalingHandler::EIgnoreAspectRatio*/)
{
    CTsGraphicFileScalingHandler *self = CTsGraphicFileScalingHandler::NewLC(aNotify,
                                                                        aImputFbsBitmap,
                                                                        aNewSize,
                                                                        aKindOfScaling);
    CleanupStack::Pop();
    return self;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C CTsGraphicFileScalingHandler* CTsGraphicFileScalingHandler::NewLC(MImageReadyCallBack &aNotify,
                                                const CFbsBitmap &aImputFbsBitmap,
                                                const TSize &aNewSize,
                                                TKindOfScaling aKindOfScaling
                                                /* = CTsGraphicFileScalingHandler::EIgnoreAspectRatio*/)
{
    CTsGraphicFileScalingHandler *self = new (ELeave) CTsGraphicFileScalingHandler(aNotify,
                                                                                aNewSize,
                                                                                aKindOfScaling);

    CleanupStack::PushL(self);
    self->ConstructL(aImputFbsBitmap);
    return self;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::ConstructL(const CFbsBitmap &aImputFbsBitmap)
{
    if (0>=mNewSize.iWidth || 0>=mNewSize.iHeight) {
       User::Leave(KErrCorrupt);
    }

    mInputBitmap = new(ELeave)CFbsBitmap();
    User::LeaveIfError(mInputBitmap->Duplicate(aImputFbsBitmap.Handle()));

    ScalingOperationL();
    SetActive();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::DoCancel()
{
    switch (mCurrentOperation) {
    case EConvertBitmapFromFile:
        mImageDecoder->Cancel();
        break;
    case EScale:
        mBitmapScaler->Cancel();
        break;
    }
    mNotify.ImageReadyCallBack(KErrCancel, 0);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::RunL()
{
    User::LeaveIfError(iStatus.Int());

    switch (mCurrentOperation) {
    case EConvertBitmapFromFile: {
            delete mImageDecoder;
            mImageDecoder = 0;

            ScalingOperationL();
            SetActive();
            break;
        }
    case EScale: {
            mCurrentOperation = ENone;

            delete mBitmapScaler;
            mBitmapScaler = 0;
            
            delete mInputBitmap;
            mInputBitmap = 0;

            if (mKindOfScaling == CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding) {
                User::LeaveIfError(mOutputBitmap->Resize(mNewSize));
            }

            mNotify.ImageReadyCallBack(iStatus.Int(), mOutputBitmap);
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::DecodingOperationL(RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType)
{
    // convert *.png to bitmap
    mImageDecoder = CImageDecoder::FileNewL(aFs, aFileName, aMimeType);
    const TFrameInfo frameInfo(mImageDecoder->FrameInfo(0));
    mInputBitmap->Reset();
    User::LeaveIfError(mInputBitmap->Create(frameInfo.iOverallSizeInPixels, 
                                               frameInfo.iFrameDisplayMode));

    mImageDecoder->Convert(&iStatus, *mInputBitmap, 0);
    mCurrentOperation = EConvertBitmapFromFile;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::ScalingOperationL()
{
    mBitmapScaler = CBitmapScaler::NewL();
    mBitmapScaler->SetQualityAlgorithm(CBitmapScaler::EMaximumQuality);

    FixForDisplayModeNotSupportedByScalingOperation();

    mOutputBitmap = new (ELeave)CFbsBitmap();
    User::LeaveIfError(mOutputBitmap->Create(NewSizeToScalingOperation(), mInputBitmap->DisplayMode()));
    mBitmapScaler->Scale(&iStatus, *mInputBitmap, *mOutputBitmap, EFalse);
    mCurrentOperation = EScale;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CTsGraphicFileScalingHandler::FixForDisplayModeNotSupportedByScalingOperation()
{
    if (EColor16MAP == mInputBitmap->DisplayMode()) {
        mInputBitmap->SetDisplayMode(EColor16MA);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TSize CTsGraphicFileScalingHandler::NewSizeToScalingOperation()
{
   TSize originalSize = mInputBitmap->SizeInPixels();
   float widthFactor = mNewSize.iWidth / (float)originalSize.iWidth;
   float heightFactor = mNewSize.iHeight / (float)originalSize.iHeight;

   TSize retSize(mNewSize);

   if (mKindOfScaling == CTsGraphicFileScalingHandler::EKeepAspectRatio) {
       retSize = (widthFactor < heightFactor) ?
                 TSize(mNewSize.iWidth, widthFactor * originalSize.iHeight) :
                 TSize(heightFactor * originalSize.iWidth, mNewSize.iHeight);
   } else if (mKindOfScaling == CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding) {
      retSize = (widthFactor < heightFactor) ?
                TSize(heightFactor * originalSize.iWidth, mNewSize.iHeight) :
                TSize(mNewSize.iWidth, widthFactor * originalSize.iHeight);
   }
   return retSize;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CTsGraphicFileScalingHandler::RunError(TInt aError)
{
    mNotify.ImageReadyCallBack(aError, 0);
    return KErrNone;
}
