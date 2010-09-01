/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "wrtdatapluginobserver.h"

CWrtDataPluginObserver::CWrtDataPluginObserver()
    {
    // No implementation required
    }

CWrtDataPluginObserver::~CWrtDataPluginObserver()
    {
    }

CWrtDataPluginObserver* CWrtDataPluginObserver::NewLC()
    {
    CWrtDataPluginObserver* self = new (ELeave) CWrtDataPluginObserver();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CWrtDataPluginObserver* CWrtDataPluginObserver::NewL()
    {
    CWrtDataPluginObserver* self = CWrtDataPluginObserver::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

void CWrtDataPluginObserver::ConstructL()
    {

    }

TInt CWrtDataPluginObserver::StartTransaction( TInt /*aTxId*/ )
    {
    return KErrNotSupported;
    }

TInt CWrtDataPluginObserver::Commit( TInt /*aTxId */)
    {
    return KErrNone;
    }


TInt CWrtDataPluginObserver::CancelTransaction( TInt /*aTxId */)
    {
    return KErrNone;
    }


TBool CWrtDataPluginObserver::CanPublish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex */)
    {
    return EFalse;
    }


TInt CWrtDataPluginObserver::Publish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/ )
    {
    return KErrNotSupported;
    }


TInt CWrtDataPluginObserver::Publish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, const TDesC16& /*aText*/, TInt /*aIndex */)
    {
    return KErrNotSupported;
    }


TInt CWrtDataPluginObserver::Publish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, const TDesC8& /*aBuf*/, TInt /*aIndex */)
    {
    return KErrNotSupported;
    }
    

TInt CWrtDataPluginObserver::Publish( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex */)
    {
    return KErrNotSupported;
    }

TInt CWrtDataPluginObserver::Clean( CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex */)
    {
    return KErrNotSupported;
    }


TAny* CWrtDataPluginObserver::Extension( TUid /*aUid */)
    {
    return NULL;
    }


TBool CWrtDataPluginObserver::RequiresSubscription( const THsPublisherInfo& /*aPublisherInfo */) const
    {
    return EFalse;
    }
    

TInt CWrtDataPluginObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
            const TDesC8& /*aElementId*/,
            const TDesC8& /*aPropertyName*/,
            const TDesC8& /*aPropertyValue */)
    {
    return KErrNotSupported;
    }
    
TInt CWrtDataPluginObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
            const TDesC8& /*aElementId*/,
            const TDesC8& /*aPropertyName*/,
            const TDesC8& /*aPropertyValue*/,  
            MAiContentObserver::TValueType /*aValueType*/)
    {
    return KErrNotSupported;
    }

