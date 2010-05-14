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
#include "activitygraphicfilescaling.h"
#include <imageconversion.h>


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CGraphicsSalingHandler::CGraphicsSalingHandler(MImageReadyCallBack &aNotify, 
                                               const TSize &aNewSize, 
                                               TKindOfScaling aKindOfScaling):
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
CGraphicsSalingHandler::~CGraphicsSalingHandler()
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
CGraphicsSalingHandler* CGraphicsSalingHandler::NewL(MImageReadyCallBack &aNotify,
                                                     RFs &aFs,
                                                     const TDesC &aFileName,
                                                     const TDesC8& aMimeType,
                                                     const TSize &aNewSize,
                                                     TKindOfScaling aKindOfScaling)
{
    CGraphicsSalingHandler *self = CGraphicsSalingHandler::NewLC(aNotify, 
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
CGraphicsSalingHandler* CGraphicsSalingHandler::NewLC(MImageReadyCallBack &aNotify,
                                              RFs &aFs,
                                              const TDesC &aFileName,
                                              const TDesC8& aMimeType,
                                              const TSize &aNewSize,
                                              TKindOfScaling aKindOfScaling)
{
    CGraphicsSalingHandler *self = new (ELeave) CGraphicsSalingHandler(aNotify, 
                                                                       aNewSize, 
                                                                       aKindOfScaling);
    CleanupStack::PushL(self);
    self->ConstructL(aFs, aFileName, aMimeType);
    return self;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CGraphicsSalingHandler::ConstructL(RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType)
{
    CActiveScheduler::Add(this);

    if (aFileName.Length() == 0
        || aFs.IsValidName(aFileName) == EFalse) {
        User::Leave(KErrPathNotFound);
    }

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
void CGraphicsSalingHandler::DoCancel()
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
void CGraphicsSalingHandler::RunL()
{
    if(iStatus.Int()){
        mNotify.ImageReadyCallBack(iStatus.Int(), 0);
        return;
    }

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
            
            mNotify.ImageReadyCallBack(iStatus.Int(), mBitmapOutput);
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TSize CGraphicsSalingHandler::Scaling()
{
   TSize originalSize = mBitmapFromFile->SizeInPixels();
   float widthFactor = mNewSize.iWidth / (float)originalSize.iWidth;
   float heightFactor = mNewSize.iHeight / (float)originalSize.iHeight;
   TSize retSize(mNewSize);
   if (mKindOfScaling == CGraphicsSalingHandler::EKeepAspectRatio) {
       retSize = (widthFactor < heightFactor) ?
                 TSize(mNewSize.iWidth, widthFactor * originalSize.iHeight) :
                 TSize(heightFactor * originalSize.iWidth, mNewSize.iHeight);
   } else if (mKindOfScaling == CGraphicsSalingHandler::EKeepAspectRatioByExpanding) {
      retSize = (widthFactor < heightFactor) ?
                TSize(heightFactor * originalSize.iWidth, mNewSize.iHeight) :
                TSize(mNewSize.iWidth, widthFactor * originalSize.iHeight);
   }
   return retSize;
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CGraphicsSalingHandler::RunError(TInt aError)
{
    mNotify.ImageReadyCallBack(aError, 0);
    return KErrNone;
}
