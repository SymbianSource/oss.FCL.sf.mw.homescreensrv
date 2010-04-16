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

#include <apgcli.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include "cautils.h"
#include "cadef.h"
#include "cainnerentry.h"
#include "cauninstalloperation.h"
_LIT8(KAppMimeType, "x-epoc/x-sisx-app");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCaUninstallOperation::~CCaUninstallOperation
// ---------------------------------------------------------------------------
//
CCaUninstallOperation::~CCaUninstallOperation()
{
    Cancel();
    iUninstaller.Close();
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::NewL
// ---------------------------------------------------------------------------
//
CCaUninstallOperation *CCaUninstallOperation::NewL(CCaInnerEntry &aEntry,
        TInt aPriority)
{
    CCaUninstallOperation *self;

    self = new(ELeave) CCaUninstallOperation(aEntry, aPriority);
    CleanupStack::PushL(self);
    self->ConstructL(aEntry);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::CCaUninstallOperation
// ---------------------------------------------------------------------------
//
CCaUninstallOperation::CCaUninstallOperation(CCaInnerEntry &aEntry,
        TInt aPriority) :
    CActive(aPriority), iEntry(aEntry), iUninstaller(), iId(0)
{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CCaUninstallOperation::ConstructL(CCaInnerEntry &aEntry)
{
    TBuf<KCaMaxAttrValueLen> uidAttr;
    TUint uid = aEntry.GetUid();

    TUid packageUid = KNullUid;
    TPtrC8 mimeType;

    if (!(aEntry.GetFlags() & ERemovable)) {
        User::Leave(KErrAccessDenied);
    }

    User::LeaveIfError(iUninstaller.Connect());
    iId = aEntry.GetId();
    if (aEntry.GetEntryTypeName() == KCaTypeWidget()) {
        TBuf<KMaxUidName> uidDesc;
        aEntry.FindAttribute(KCaPackageUid, uidDesc);
        TLex uidLex(uidDesc);
        TUint32 uidValue;
        User::LeaveIfError(uidLex.Val(uidValue, EHex));

        packageUid.iUid = static_cast<TInt32>(uidValue);
        mimeType.Set(KAppMimeType);
    } else {
        AppInfoL(TUid::Uid(uid), mimeType, packageUid);
    }

    // Commence the uninstallations
    iUninstaller.Uninstall(iStatus, packageUid, mimeType);
    SetActive();
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::AppInfo
// ---------------------------------------------------------------------------
//
void CCaUninstallOperation::AppInfoL(const TUid &aAppUid,
                                     TPtrC8 &aMimeType, TUid &aPackageUid)
{
    RApaLsSession apaLsSession;

    User::LeaveIfError(apaLsSession.Connect());
    CleanupClosePushL(apaLsSession);
    User::LeaveIfError(apaLsSession.GetAllApps());

    TApaAppInfo appInfo;
    User::LeaveIfError(apaLsSession.GetAppInfo(appInfo, aAppUid));
    if (!GetInstallPkgUidL(appInfo.iFullName, aPackageUid)) {
        aPackageUid = aAppUid;
    }
    aMimeType.Set(KAppMimeType);

    CleanupStack::PopAndDestroy(&apaLsSession);
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::GetInstallPkgUidL
// ---------------------------------------------------------------------------
//
TBool CCaUninstallOperation::GetInstallPkgUidL(const TDesC &aAppFullName,
        TUid &aPackageUid)
{
    // Search for the full name of the application amongst every file name in
    // every installed packages.
    TBool found = EFalse;
    Swi::RSisRegistrySession iSisRegSession;

    // Get the array of ids of every installed packages
    if (KErrNone != iSisRegSession.Connect()) {
        return found;
    }
    CleanupClosePushL(iSisRegSession);

    RArray<TUid> packageIds;
    CleanupClosePushL(packageIds);

    iSisRegSession.InstalledUidsL(packageIds);

    RPointerArray<HBufC> packageFiles;
    CleanupClosePushL(packageFiles);

    for (TInt i = 0; i < packageIds.Count() && !found; ++i) {
        const TUid packageId = packageIds[i];
        Swi::RSisRegistryEntry packageEntry;

        // Get the array of file names in the current install package and look
        // if there is one suggesting that the application was installed from
        // the package.
        if (KErrNone == packageEntry.Open(iSisRegSession, packageId)) {
            CleanupClosePushL(packageEntry);
            packageEntry.FilesL(packageFiles);
            for (TInt pf = 0; pf < packageFiles.Count() && !found; ++pf) {
                if (packageFiles[pf]->FindC(aAppFullName) == 0) {
                    aPackageUid = packageId;
                    found = ETrue;
                }
            }
            packageFiles.ResetAndDestroy();
            CleanupStack::PopAndDestroy(&packageEntry);
        }
    }

    CleanupStack::PopAndDestroy(&packageFiles);
    CleanupStack::PopAndDestroy(&packageIds);
    CleanupStack::PopAndDestroy(&iSisRegSession);

    return found;
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::RunL
// ---------------------------------------------------------------------------
//
void CCaUninstallOperation::RunL()
{
    iUninstaller.Close();
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::DoCancel
// ---------------------------------------------------------------------------
//
void CCaUninstallOperation::DoCancel()
{
    iUninstaller.CancelAsyncRequest(SwiUI::ERequestUninstall);
}

// ---------------------------------------------------------------------------
// CCaUninstallOperation::RunError
// ---------------------------------------------------------------------------
//
TInt CCaUninstallOperation::RunError(TInt /* aError */)
{
    return KErrNone;
}
