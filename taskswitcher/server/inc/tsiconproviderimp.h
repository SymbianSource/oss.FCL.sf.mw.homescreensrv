/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Task list entry
*
*/

#ifndef TSICONPROVIDERIMP_H
#define TSICONPROVIDERIMP_H

#include <e32base.h>

#include "tsiconprovider.h"
#include "tsresourcemanager.h"

class QIcon;

class CTsIconProvider: public CBase,
                       public MTsIconProvider
    {
public:
    static CTsIconProvider* NewL(MTsResourceManager& aResources);
    ~CTsIconProvider();
    void LoadIconL(CFbsBitmap& aDst, TUid aUid);

private:
    enum TIconSource
        {
        ESkin,
        EApparc,
        EFile,
        EDefault
        };
    CTsIconProvider(MTsResourceManager& aResources);
    void ConstructL();
    CFbsBitmap* Bitmap( const QIcon& aIcon );
    CFbsBitmap* Bitmap( CFbsBitmap& aBitmap, CFbsBitmap& aMask );
    
    void LoadL(TIconSource aSrc, CFbsBitmap& aDst, TUid aUid);
    void LoadIconFromSkinL(CFbsBitmap& aDst, TUid aUid);
    void LoadIconFromApparcL(CFbsBitmap& aDst, TUid aUid);
    void LoadIconFromFileL(CFbsBitmap& aDst, TUid aUid);
    void LoadIconDefaultL(CFbsBitmap& aDst);
    void CopyAndDestroyL(CFbsBitmap& aDst, CFbsBitmap* aSrc);

private:
    MTsResourceManager& iResources;
    CFbsBitmap* iDefaultIcon;
    };

#endif //TSICONPROVIDER_H
