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
* Description:  ?Description

 *
*/


#include <liwcommon.h>
#include <liwvariant.h>
#include <s32mem.h>

#include "cpactiondatacache.h"
#include "cpglobals.h"

using namespace LIW;

_LIT8(KCachedMap, "cached_map");

static const int KMaxCacheItems = 6; 


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionDataCache* CCPActionDataCache::NewL()
    {
    CCPActionDataCache* self = CCPActionDataCache::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionDataCache* CCPActionDataCache::NewLC()
    {
    CCPActionDataCache* self = new (ELeave) CCPActionDataCache;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionDataCache::ConstructL( )
    {
    iInternalList = CLiwDefaultList::NewL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionDataCache::CCPActionDataCache()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionDataCache::~CCPActionDataCache()
    {
    iInternalList->Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionDataCache::HandleCacheRemoveL(const CLiwMap* aMap)
    {
    TInt id = FindL(aMap);
    if (id != KErrNotFound)
        {
        iInternalList->Remove(id);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionDataCache::AppendL( const CLiwGenericParamList* aParamList)
    {
    const TLiwGenericParam* param;
    TInt pos( 0);
    param = aParamList->FindFirst( pos, KListMap );

    if (param && param->Value().TypeId() == EVariantTypeMap)
        {
        const CLiwMap* inputMap = param->Value().AsMap();
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
        CopyVariantL(KId, inputMap, map);
        CopyVariantL(KPublisherId, inputMap, map);
        CopyVariantL(KContentType, inputMap, map);
        CopyVariantL(KContentId, inputMap, map);
        map->InsertL(KCachedMap, TLiwVariant(inputMap));
        iInternalList->AppendL(TLiwVariant(map));
        CleanupStack::PopAndDestroy(map);
    
        if (iInternalList->Count() > KMaxCacheItems)
            {
            iInternalList->Remove(0);
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPActionDataCache::IsCacheableL(const CLiwMap* aMap)
    {
    TBool result(EFalse);
    TLiwVariant value;
    value.PushL();

    if (aMap->FindL(KId, value) && value.AsTInt32() > 0)
        {
        result = ETrue;
        }
    else if (IsSpecifiedL(aMap, KPublisherId) && IsSpecifiedL(aMap,
            KContentType) && IsSpecifiedL(aMap, KContentId))
        {
        result = ETrue;
        }

    CleanupStack::PopAndDestroy(&value);
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPActionDataCache::ExistL(const CLiwMap* aMap)
    {
    TBool result(EFalse);
    if (FindL(aMap) != KErrNotFound)
        {
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionDataCache::GetL(const CLiwMap* aMap,
        CLiwGenericParamList* aParamList)
    {
    TInt id = FindL(aMap);
    if (id != KErrNotFound)
        {
        TLiwVariant value;
        value.PushL();
        iInternalList->AtL(id, value);
        const CLiwMap* map = value.AsMap();
        if (map->FindL(KCachedMap, value))
            {
            CLiwDefaultMap* outMap = CLiwDefaultMap::NewLC();
            value.Get(*outMap);
            TLiwGenericParam genericParam(KListMap, TLiwVariant(outMap));
            aParamList->AppendL(genericParam);
            CleanupStack::PopAndDestroy(outMap);
            }
        CleanupStack::PopAndDestroy(&value);
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPActionDataCache::MatchL(const CLiwMap* aCachedMap,
        const CLiwMap* aInputMap)
    {
    TBool idMatch(EFalse);
    TLiwVariant l, r;
    l.PushL();
    r.PushL();
    if (aCachedMap->FindL(KId, l) && aInputMap->FindL(KId, r) && l.AsTInt32()
            == r.AsTInt32())
        {
        idMatch = ETrue;
        }
    if (!idMatch)
        {
        if (MatchL(aCachedMap, aInputMap, KPublisherId) && MatchL(aCachedMap,
                aInputMap, KContentType) && MatchL(aCachedMap, aInputMap,
                KContentId))
            {
            idMatch = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(&r);
    CleanupStack::PopAndDestroy(&l);
    return idMatch;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPActionDataCache::MatchL(const CLiwMap* aLeft, const CLiwMap* aRight,
        const TDesC8& aKey)
    {
    TBool match(EFalse);
    TLiwVariant l, r;
    l.PushL();
    r.PushL();
    if (aLeft->FindL(aKey, l) && aRight->FindL(aKey, r)
            && !l.AsDes().Compare(r.AsDes()))
        {
        match = ETrue;
        }
    CleanupStack::PopAndDestroy(&r);
    CleanupStack::PopAndDestroy(&l);
    return match;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPActionDataCache::IsSpecifiedL(const CLiwMap* aMap,
        const TDesC8& aKey)
    {
    TBool result(EFalse);
    TLiwVariant value;
    value.PushL();
    if (aMap->FindL(aKey, value) && value.AsDes().Compare(KNullDesC) != 0
            && value.AsDes().Compare(KAll) != 0)
        {
        result = ETrue;
        }
    CleanupStack::PopAndDestroy(&value);
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCPActionDataCache::FindL(const CLiwMap* aKey)
    {
    TInt result(KErrNotFound);
    TLiwVariant value;
    value.PushL();
    for (TInt i = 0; i < iInternalList->Count(); i++)
        {
        iInternalList->AtL(i, value);
        if (MatchL(value.AsMap(), aKey))
            {
            result = i;
            break;
            }
        }
    CleanupStack::PopAndDestroy(&value);
    return result;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCPActionDataCache::CopyVariantL(const TDesC8& aKey,
        const CLiwMap* aInMap, CLiwDefaultMap* aOutMap)
    {
    //TODO: method exist also in data manager - should be refactored
    TLiwVariant variant;
    variant.PushL();
    if (aInMap->FindL(aKey, variant))
        {
        aOutMap->InsertL(aKey, variant);
        }
    CleanupStack::PopAndDestroy(&variant);
    }
