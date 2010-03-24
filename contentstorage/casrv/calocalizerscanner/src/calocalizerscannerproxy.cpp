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

const char* KEmptyString = "";

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
    iTranslator = new ( ELeave ) QTranslator();
    PerformL();
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
    delete iTranslator;
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::PerformL
// ---------------------------------------------------------------------------
//
void CCaLocalizerScannerProxy::PerformL()
    {
    QString locale = QLocale::system().name();
    QString filename = QString( "contentstorage_" ) + locale;
    LoadTranslator( filename );
    TPtrC ptrLocale( reinterpret_cast<const TText*>( locale.constData() ) );
    TBuf<KCaMaxAttrNameLen> propertyValue;
    // trap is here to assure deletion of qtranslator in case code leaves
    iStorageProxy->DbPropertyL( KCaDbPropLanguage, propertyValue );
    if( ptrLocale.CompareC( propertyValue ) )
        {
        // language is changed - update locale names
        UpdateLocalNamesL();
        // remember info about new language in db
        iStorageProxy->SetDBPropertyL( KCaDbPropLanguage, ptrLocale );
        }
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::LoadTranslator
// ---------------------------------------------------------------------------
//
TBool CCaLocalizerScannerProxy::LoadTranslator( QString filename )
    {
    TBool loaded( false );
    // load from rom or testbase 
    loaded = iTranslator->load( filename, QString( "z:/resource/qt/translations" ) );
    if( !loaded )
        {
        loaded = iTranslator->load( filename, QString( "c:/resource/qt/translations" ) );
        }

    return loaded;
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
    GetLocalizationRowsL( locals );
    QString locName;
    RBuf localizedName;
    CleanupClosePushL( localizedName );
    localizedName.CreateL( KCaMaxAttrValueLen ); 

    for( TInt i = 0; i < locals.Count(); i++ )
        {
        const char* temp = DescriptorToStringL( locals[i]->GetStringId() );
        // first string is a contex, probably to put in database in future
        locName = iTranslator->translate( KEmptyString, temp, KEmptyString );
        localizedName = reinterpret_cast<const TText*> ( locName.constData() );
        if (localizedName.Compare(KNullDesC))
        	{
        	locals[i]->SetLocalizedStringL( localizedName );
        	}
        else
        	{
        	locals[i]->SetLocalizedStringL( locals[i]->GetStringId() );  
        	}
        iStorageProxy->LocalizeEntryL( *( locals[i] ) );
        delete[] temp;
        }
    CleanupStack::PopAndDestroy( &localizedName );
    CleanupStack::PopAndDestroy( &locals );
    }

// ---------------------------------------------------------------------------
// CCaLocalizerScannerProxy::DescriptorToStringL
// ---------------------------------------------------------------------------
//
const char* CCaLocalizerScannerProxy::DescriptorToStringL(
        const TDesC& aDescriptor )
    {
    TInt length = aDescriptor.Length();
    HBufC8* buffer = HBufC8::NewLC( length );
    buffer->Des().Copy( aDescriptor );
    char* str = new ( ELeave ) char[length + 1];
    Mem::Copy( str, buffer->Ptr(), length );
    str[length] = '\0';
    CleanupStack::PopAndDestroy( buffer );
    return str;
    }

