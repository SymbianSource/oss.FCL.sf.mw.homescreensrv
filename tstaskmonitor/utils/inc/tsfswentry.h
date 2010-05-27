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


#ifndef TSFSWENTRY_H
#define TSFSWENTRY_H

#include <e32base.h>
#include <e32cmn.h>

class CTsFswEntry;
class RReadStream;
class RWriteStream;
class CFbsBitmap;

typedef RPointerArray<CTsFswEntry> RTsFswArray;

enum TAppType
{
    EApp = 0,
    EWidget,
    EJava    
};

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsFswEntry ) : public CBase
    {
public:
    static CTsFswEntry* NewL();
    static CTsFswEntry* NewLC();
    ~CTsFswEntry();

    /**
     * Window group id.
     * In case of wrt widgets this is negative and is not a valid window group id.
     * Use this when calling CloseApp or SwitchToApp. (these work with widgets too)
     */
    TInt WgId() const;
    
    /**
     * Application uid.
     */
    TUid AppUid() const;
    
    /**
     * Application type.
     */
    TAppType Type() const;
    
    /**
     * Application name.
     */
    const TDesC& AppName() const;    
    
    /**
     * ETrue if the application is closeable
     */
    TBool CloseableApp() const;
    
    /**
     * Application icon bitmap
     */
    CFbsBitmap* AppIconBitmap() const;
    
    /**
     * Application icon mask.
     * @see AppIconBitmapHandle
     */
    CFbsBitmap* AppIconMask() const;


    void SetWgId( TInt aWgId );
    void SetAppUid( const TUid& aUid );
    void SetAppNameL( const TDesC& aAppName );
    void SetCloseableApp( TBool aValue );
    
    // takes ownership of bitmaps
    void SetAppIcon( CFbsBitmap* aBitmap, CFbsBitmap* aMask );
    void ExternalizeL( RWriteStream& aStream );
    void InternalizeL( RReadStream& aStream );

    static void ExternalizeArrayL( RWriteStream& aStream,
        const RTsFswArray& aArray );
    static void InternalizeArrayL( RReadStream& aStream,
        RTsFswArray& aArray );

private:
    TInt iWgId;
    TUid iAppUid;
    HBufC* iAppName;
    TBool iCloseableApp;
    CFbsBitmap* iAppIconBitmap;
    CFbsBitmap* iAppIconMask;  
    };

#endif
