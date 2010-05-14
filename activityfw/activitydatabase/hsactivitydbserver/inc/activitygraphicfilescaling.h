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
#ifndef ACTIVITYGRAPHICFILESSCALING_H
#define ACTIVITYGRAPHICFILESSCALING_H

#ifndef __E32SVR_H__
#define __E32SVR_H__
#endif


#ifndef SYMBIAN_ENABLE_PUBLIC_PLATFORM_HEADER_SPLIT
#define SYMBIAN_ENABLE_PUBLIC_PLATFORM_HEADER_SPLIT
#endif

#include <bitmaptransforms.h>

class CImageDecoder;
class CFbsBitmap;

class MImageReadyCallBack
{
public:
    virtual void ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap ) = 0;
};

class CGraphicsSalingHandler : public CActive
{
public:

enum TCurrentOperation{
        ENone = 0,
        EConvertBitmapFromFile = 1,
        EScale = 2
    };

enum TKindOfScaling
{
   EIgnoreAspectRatio = 0,
   EKeepAspectRatio = 1,
   EKeepAspectRatioByExpanding = 2,
};

    ~CGraphicsSalingHandler();

    static CGraphicsSalingHandler* NewL(MImageReadyCallBack &aNotify, RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType, const TSize &aNewSize, TKindOfScaling aKindOfScaling);
    static CGraphicsSalingHandler* NewLC(MImageReadyCallBack &aNotify, RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType, const TSize &aNewSize, TKindOfScaling aKindOfScaling);

protected:
    void DoCancel();
    void RunL();
    TInt RunError(TInt);

private:
    void ConstructL(RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType);
    CGraphicsSalingHandler(MImageReadyCallBack &aNotify, const TSize &aNewSize, TKindOfScaling aKindOfScaling);
    TSize Scaling();

private:
    MImageReadyCallBack &mNotify;
    CImageDecoder *mImageDecoder;
    CFbsBitmap *mBitmapFromFile;
    CFbsBitmap *mBitmapOutput;
    CBitmapScaler *mBitmapScaler;
    TSize mNewSize;
    TBool mKindOfScaling;

    TCurrentOperation mCurrentOperation;
};

#endif // ACTIVITYGRAPHICFILESSCALING_H
