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
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
EXPORT_C CTsGraphicFileScalingHandler::~CTsGraphicFileScalingHandler()
{
    Cancel();
    delete mBitmapFromFile;
    delete mBitmapOutput;
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

    if(0>=mNewSize.iWidth || 0>=mNewSize.iHeight) {
       User::Leave(KErrCorrupt);
    }

    CActiveScheduler::Add(this);

    mBitmapScaler = CBitmapScaler::NewL();
    mBitmapScaler->SetQualityAlgorithm(CBitmapScaler::EMaximumQuality);

    // convert *.png to bitmap
    mImageDecoder = CImageDecoder::FileNewL(aFs, aFileName, aMimeType);
    mBitmapFromFile = new(ELeave)CFbsBitmap();
    const TFrameInfo frameInfo(mImageDecoder->FrameInfo(0));
    User::LeaveIfError(mBitmapFromFile->Create(frameInfo.iOverallSizeInPixels, 
                                               frameInfo.iFrameDisplayMode));

    mImageDecoder->Convert(&iStatus, *mBitmapFromFile, 0);
    mCurrentOperation = EConvertBitmapFromFile;
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
            
            mBitmapOutput = new (ELeave)CFbsBitmap();
            User::LeaveIfError(mBitmapOutput->Create(Scaling(), mBitmapFromFile->DisplayMode()));
            mBitmapScaler->Scale(&iStatus, *mBitmapFromFile, *mBitmapOutput, EFalse);
            mCurrentOperation = EScale;
            SetActive();
            break;
        }
    case EScale: {
            mCurrentOperation = ENone;

            delete mBitmapScaler;
            mBitmapScaler = 0;
            
            delete mBitmapFromFile;
            mBitmapFromFile = 0;

            if (mKindOfScaling == CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding) {
                User::LeaveIfError(mBitmapOutput->Resize(mNewSize));
            }

            mNotify.ImageReadyCallBack(iStatus.Int(), mBitmapOutput);
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TSize CTsGraphicFileScalingHandler::Scaling()
{
   TSize originalSize = mBitmapFromFile->SizeInPixels();
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
