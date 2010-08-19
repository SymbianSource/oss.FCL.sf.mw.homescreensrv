/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef C_CONTENTCACHE_H
#define C_CONTENTCACHE_H

class CContentCache : public CBase
    {
    public:
    ~CContentCache()
        {
        delete iText;
        delete iData;
        }
    
    HBufC* iText;
    HBufC8* iData;
    TBool iClean;
    TInt iResource;
    TInt iIndex;
    TInt iId;
    };


LOCAL_C CContentCache* GetContent( RPointerArray<CContentCache>& aArray, TInt aId, TBool aClean = EFalse )
    {
    const TInt count( aArray.Count() );
    for( TInt i( 0 ); i < count; i++ )
        {
        if( aArray[i]->iId == aId )
            {
            if( !aClean && aArray[i]->iClean)
                {
                continue;
                }
            return aArray[i];
            }
        }
        
    return NULL;
    }


#endif      //  C_CONTENTCACHE_H

// End of file
