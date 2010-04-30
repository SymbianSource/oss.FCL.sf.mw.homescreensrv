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
 * Description: CCaLocalizerScannerProxy.cpp
 *
 */

#include <badesca.h>
#include <f32file.h>
#include <qtranslator.h>
#include <calocalizationentry.h>
#include <QLocale>

//#include "cainternaltypes.h"
#include "caarraycleanup.inl"
#include "calocalizerscannerproxy.h"
#include "castorageproxy.h"
#include "cadef.h"
#include "cainnerquery.h"
#include "cainnerentry.h"

_LIT(KPathLoc,"z:/resource/qt/translations");

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCaLocalizerScannerProxy* CCaLocalizerScannerProxy::NewL(
        CCaStorageProxy* aStorageProxy )
    {
    CCaLocalizerScannerProxy* self = CCaLocalizerScannerProxy::NewLC(
        aStorageProxy );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::NewLC
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCaLocalizerScannerProxy* CCaLocalizerScannerProxy::NewLC(
        CCaStorageProxy* aStorageProxy )
    {
    CCaLocalizerScannerProxy* self = new( ELeave ) CCaLocalizerScannerProxy(
        aStorageProxy );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCaLocalizerScannerProxy::ConstructL()
    {
    TBuf<KCaMaxAttrNameLen> filenameDsc;
    iStorageProxy->DbPropertyL( KCaDbPropQMfile, filenameDsc );
    iResolver = new (ELeave) HbTextResolverSymbian;
    iResolver->Init(filenameDsc, KPathLoc);
    UpdateLocalNamesL();
    delete iResolver;
    iResolver = NULL;
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::CCaLocalizerScannerProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CCaLocalizerScannerProxy::CCaLocalizerScannerProxy(
        CCaStorageProxy* aStorageProxy ) :
    iStorageProxy( aStorageProxy )
    {
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::~CCaLocalizerScannerProxy
// Destructor.
// ---------------------------------------------------------------------------
//
CCaLocalizerScannerProxy::~CCaLocalizerScannerProxy()
    {
	if (iResolver)
		{
	    delete iResolver;
		} 
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::GetLocalizationRowsL
// ---------------------------------------------------------------------------
//
void CCaLocalizerScannerProxy::GetLocalizationRowsL( RPointerArray<
        CCaLocalizationEntry>& aLocals )
    {
    if( iStorageProxy )
        {
        iStorageProxy->GetLocalizationsL( aLocals );
        }
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::UpdateLocalNamesL
// ---------------------------------------------------------------------------
//
void CCaLocalizerScannerProxy::UpdateLocalNamesL()
    {
    RPointerArray<CCaLocalizationEntry> locals;
    CleanupResetAndDestroyPushL( locals );
    RPointerArray<CCaInnerEntry> entries;
    CleanupResetAndDestroyPushL( entries );
    RArray<TInt> ids;
    CleanupClosePushL( ids );
    
    GetLocalizationRowsL( locals );
     
    TInt locCount = locals.Count();
    for( TInt idx = 0; idx < locCount; idx++ )
        {
        ids.Append( locals[idx]->GetRowId() );
        }    
    CCaInnerQuery* query = CCaInnerQuery::NewLC();
    query->SetIdsL( ids );
    iStorageProxy->GetEntriesL( query, entries ); 
    CleanupStack::PopAndDestroy( query );
    
    HBufC16* localizedName;
    for( TInt i = 0; i < locCount; i++ )
        {
        localizedName = iResolver->LoadLC( locals[i]->GetStringId() );
        if( localizedName->Compare(
            GetEntryText( entries, locals[i]->GetRowId() ) ) )
            // translation different than text
            {
            locals[i]->SetLocalizedStringL( *localizedName );
            iStorageProxy->LocalizeEntryL( *( locals[i] ) );
            } 
        else if( !localizedName->Compare(KNullDesC) ) 
            // no translation, string id as text
            {
            locals[i]->SetLocalizedStringL( locals[i]->GetStringId() );
            iStorageProxy->LocalizeEntryL( *( locals[i] ) );
            }
        CleanupStack::PopAndDestroy( localizedName );
        }
   
    CleanupStack::PopAndDestroy( &ids );
    CleanupStack::PopAndDestroy( &entries );
    CleanupStack::PopAndDestroy( &locals );
    }


// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::LocalGetEntryById
// ---------------------------------------------------------------------------
//
const TDesC& CCaLocalizerScannerProxy::GetEntryText(
        RPointerArray<CCaInnerEntry> aEntries, TInt aId )
    {
    TInt entriesCount = aEntries.Count();
    for( TInt i=0; i < entriesCount; i++ )
        {
        if( aEntries[i]->GetId() == aId )
            {
            return aEntries[i]->GetText();
            }
        }
    return KNullDesC();
    }
