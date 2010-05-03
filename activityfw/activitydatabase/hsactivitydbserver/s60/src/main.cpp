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
* Description: Application entry point
*
*/

#include <QtCore>
#include <QApplication>
#include <qs60mainapplication.h>
#include <qs60mainappui.h>
#include <qs60maindocument.h>

#include <eikenv.h>
#include <apgwgnam.h>

#include "hsactivityserver.h"
#include "hsactivitystorage.h"

class HsActivityMainAppUi : public QS60MainAppUi
{
public:
    TBool FrameworkCallsRendezvous() const {
        return EFalse;
    }
};

class HsActivityMainDocument : public QS60MainDocument
{
public:
    HsActivityMainDocument(CEikApplication &mainApplication):QS60MainDocument(mainApplication) {}

    CEikAppUi *CreateAppUiL() {
        return (static_cast <CEikAppUi *>(new(ELeave)HsActivityMainAppUi));
    }
};

class HsActivityApplication : public QS60MainApplication
{
protected:
    CApaDocument *CreateDocumentL() {
        return new(ELeave) HsActivityMainDocument(*this);
    }
};

CApaApplication *newHsActivityApplication()
{
    return new HsActivityApplication;
}

int main(int argc, char *argv[])
{
    QApplication app(newHsActivityApplication, argc, argv);
    HsActivityStorage storage;
    HsActivityServer server(storage);
    int retVal(KErrGeneral);
    if (server.start()) {
        CEikonEnv *env = CEikonEnv::Static();
        if (env) {
            CApaWindowGroupName *wgName = CApaWindowGroupName::NewLC(env->WsSession());
            wgName->SetHidden(ETrue); // hides us from FSW and protects us from OOM FW etc.
            wgName->SetSystem(ETrue); // Allow only application with PowerManagement cap to shut us down
            RWindowGroup &rootWindowGroup = env->RootWin();
            wgName->SetWindowGroupName(rootWindowGroup);
            rootWindowGroup.SetOrdinalPosition(-1, ECoeWinPriorityNormal); //move to background.
            CleanupStack::PopAndDestroy();
        }
        RProcess::Rendezvous(KErrNone);
        retVal = app.exec();
    } else
        RProcess::Rendezvous(retVal);
    return retVal;
}


