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
#ifndef TSGRAPHICFILESCALINGHANDLER_H
#define TSGRAPHICFILESCALINGHANDLER_H

#include <e32base.h>

class RFs;
class CImageDecoder;
class CFbsBitmap;
class CBitmapRotator;
class CBitmapScaler;

/**
 * Interface to observer contain
 * ImageReadyCallBack function return error code and scaled/resized bitmap.
 */
class MImageReadyCallBack
    {
public:
    virtual void ImageReadyCallBack( TInt aError, const CFbsBitmap* aBitmap ) = 0;
    };

/**
 * Class to scaling graphic file/s.
 */
class CTsGraphicFileScalingHandler : public CActive
    {
public:
    /**
     * Kind of graphic file scaling.
     * EIgnoreAspectRatio          - the file is scaled to size, aspect ratio isn't preserving.
     * EKeepAspectRatio            - the file is scaled to a rectangle as large as possible inside size
     *                               preserving the aspect ratio.
     * EKeepAspectRatioByExpanding - the file is scaled to a rectangle as small as possible outside size
     *                               preserving the aspect ratio.
     */
    enum TKindOfScaling
        {
        EIgnoreAspectRatio = 0,
        EKeepAspectRatio = 1,
        EKeepAspectRatioByExpanding = 2,
        };

private:
    /**
     * Active object current operation.
     */
    enum TCurrentOperation{
        ENone = 0,
        EConvertBitmapFromFile,
        ERotate,
        EScale
    };

public:
    ~CTsGraphicFileScalingHandler();

    static CTsGraphicFileScalingHandler* NewL(MImageReadyCallBack &aNotify,
        RFs &aFs,
        const TDesC &aFileName,
        const TDesC8& aMimeType,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio,
        TInt aRotation = 0);
    
    static CTsGraphicFileScalingHandler* NewLC(MImageReadyCallBack &aNotify,
        RFs &aFs,
        const TDesC &aFileName,
        const TDesC8& aMimeType,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio,
        TInt aRotation = 0);

    
    static CTsGraphicFileScalingHandler* NewL(MImageReadyCallBack &aNotify,
        const CFbsBitmap &aInputFbsBitmap,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio,
        TInt aRotation = 0);
     static CTsGraphicFileScalingHandler* NewLC(MImageReadyCallBack &aNotify,
        const CFbsBitmap &aInputFbsBitmap,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio,
        TInt aRotation = 0);

protected:
    void DoCancel();
    void RunL();
    TInt RunError(TInt);

private:
    void ConstructL( RFs& aFs, const TDesC& aFileName, const TDesC8& aMimeType );
    void ConstructL( const CFbsBitmap& aInputFbsBitmap );
    CTsGraphicFileScalingHandler( MImageReadyCallBack& aNotify,
                                  const TSize& aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio,
        TInt aRotation = 0);
    void DecodingOperationL( RFs& aFs, const TDesC& aFileName, const TDesC8& aMimeType );

    void ScalingOperationL();

    void RotationOperationL();
    
    TInt RotationMode()const;
    
    TBool IsSupportedRotationMode() const;
    
    TSize NewSizeToScalingOperation();

    void FixForDisplayModeNotSupportedByScalingOperation();

private:
    /**
     * Reference to observer implementation - return error code and output bitmap.
     */
    MImageReadyCallBack& iNotify;
    /**
     * Pointer to decoder used by decoding graphic file algorithm.
     */
    CImageDecoder* iImageDecoder;
    /**
     * Pointer to input bitmap - before decoding/scaling operation/s.
     */
    CFbsBitmap* iInputBitmap;
    /**
     * Pointer to output bitmap - returned in mNotify object.
     */
    CFbsBitmap* iOutputBitmap;
    /**
     * Pointer to CBitmapScaler calss used by scaling graphic file algorithm.
     */
    CBitmapScaler* iBitmapScaler;
    
    CBitmapRotator* iBitmapRotator;
    /**
     * New size of output graphic file.
     */
    TSize iNewSize;
    /**
     * Kind of graphic file scaling described above.
     */
    TBool iKindOfScaling;
    
    const TInt iRotation;
    /**
     * Active object current operation.
     */
    TCurrentOperation iCurrentOperation;
    };

#endif // TSGRAPHICFILESCALINGHANDLER_H
