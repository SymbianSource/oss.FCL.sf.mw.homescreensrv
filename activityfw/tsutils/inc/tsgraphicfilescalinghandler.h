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

#ifndef __E32SVR_H__
#define __E32SVR_H__
#endif


#ifndef SYMBIAN_ENABLE_PUBLIC_PLATFORM_HEADER_SPLIT
#define SYMBIAN_ENABLE_PUBLIC_PLATFORM_HEADER_SPLIT
#endif

#include <bitmaptransforms.h>

class CImageDecoder;
class CFbsBitmap;

/**
 * Interface to observer contain
 * ImageReadyCallBack function return error code and scaled/resized bitmap.
 */
class MImageReadyCallBack
{
public:
    virtual void ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap) = 0;
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
        EConvertBitmapFromFile = 1,
        EScale = 2
    };

public:
    /**
    * Destructor.
    */
    IMPORT_C ~CTsGraphicFileScalingHandler();

    /**
     * All constructors initialise active object asynchronous operation
     * by calling ConstructL function.
     * 1st group - decoding and scaling.
     * 2nd group - scaling.
     */

    /**
     * 1st group.
     * Exported from dll constructors for activation graphic file scaling.
     * aNotify        - reference to observer implementation.
     * aFs            - reference to file server session.
     * aFileName      - path to graphic file.
     * aMimeType      - mime type of graphic file.
     * aNewSize       - new size of output graphic file.
     * aKindOfScaling - kind of graphic file scaling described above.
     */
    IMPORT_C static CTsGraphicFileScalingHandler* NewL(MImageReadyCallBack &aNotify,
        RFs &aFs,
        const TDesC &aFileName,
        const TDesC8& aMimeType,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio);
    IMPORT_C static CTsGraphicFileScalingHandler* NewLC(MImageReadyCallBack &aNotify,
        RFs &aFs,
        const TDesC &aFileName,
        const TDesC8& aMimeType,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio);

    /**
     * 2nd group.
     * Exported from dll constructors for activation graphic file scaling.
     * aNotify         - reference to observer implementation.
     * aImputFbsBitmap - reference to pattern CFbsBitmap.
     * aNewSize        - new size of output graphic file.
     * aKindOfScaling  - kind of graphic file scaling described above.
     */
    IMPORT_C static CTsGraphicFileScalingHandler* NewL(MImageReadyCallBack &aNotify,
        const CFbsBitmap &aImputFbsBitmap,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio);
     IMPORT_C static CTsGraphicFileScalingHandler* NewLC(MImageReadyCallBack &aNotify,
        const CFbsBitmap &aImputFbsBitmap,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio);

protected:
     /**
      * Cancels the wait for completion of an outstanding request.
      */
    void DoCancel();
    /**
     * Handles an active object’s request completion event.
     */
    void RunL();
    /**
     * Handles a leave occurring in the request completion event handler RunL().
     */
    TInt RunError(TInt);

private:
    /**
     * Functions construct active objest instance and made asynchronous operation/s.
     * Parameters - the same meaning as in appropriate NewL/NewLC functions.
     */
    void ConstructL(RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType);
    void ConstructL(const CFbsBitmap &aImputFbsBitmap);
    /**
     * Private constructor.
     * Parameters - the same meaning as in appropriate NewL/NewLC functions.
     */
    CTsGraphicFileScalingHandler(MImageReadyCallBack &aNotify,
        const TSize &aNewSize,
        TKindOfScaling aKindOfScaling = CTsGraphicFileScalingHandler::EIgnoreAspectRatio);
    /**
     * Action to made before decoding graphic file operation.
     * Parameters - the same meaning as in appropriate NewL/NewLC functions.
     */
    void DecodingOperationL(RFs &aFs, const TDesC &aFileName, const TDesC8& aMimeType);
    /**
     * Action to made before scaling graphic file operation.
     */
    void ScalingOperationL();
    /**
     * Algorithm to determine output bitmap (returned in ImageReadyCallBack) size
     * after scaling operation.
     */
    TSize NewSizeToScalingOperation();
    /**
     * Fix for TDisplayMode == EColor16MAP not supported by scaling operation!
     * ! ! ! ADD OTHER NOT SUPPORTED DISPLAY MODES ! ! !
     */
    void FixForDisplayModeNotSupportedByScalingOperation();

private:
    /**
     * Reference to observer implementation - return error code and output bitmap.
     */
    MImageReadyCallBack &mNotify;
    /**
     * Pointer to decoder used by decoding graphic file algorithm.
     */
    CImageDecoder *mImageDecoder;
    /**
     * Pointer to input bitmap - before decoding/scaling operation/s.
     */
    CFbsBitmap *mInputBitmap;
    /**
     * Pointer to output bitmap - returned in mNotify object.
     */
    CFbsBitmap *mOutputBitmap;
    /**
     * Pointer to CBitmapScaler calss used by scaling graphic file algorithm.
     */
    CBitmapScaler *mBitmapScaler;
    /**
     * New size of output graphic file.
     */
    TSize mNewSize;
    /**
     * Kind of graphic file scaling described above.
     */
    TBool mKindOfScaling;
    /**
     * Active object current operation.
     */
    TCurrentOperation mCurrentOperation;
};

#endif // TSGRAPHICFILESCALINGHANDLER_H
