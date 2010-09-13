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
 * Description:
 *
 */

// conversion
#include <XQConversions>
// apparc
#include <apparc.h>
#include <apgcli.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <apaidpartner.h>
#else
#include <apaid.h>
#endif
// cfbsbitmap
#include <apgicnfl.h>

#include <HbIcon>

#include "camenuiconutility.h"
#include "cabitmapadapter.h"
#include "caentry.h"
#include "caquery.h"
#include "caservice.h"
#include "cadef.h"

const char appUidAttributeName[] = "application:uid";

/*!
 Get icon from entry.
 \param entry const reference to CaEntry.
 \retval icon.
 */
LOCAL_C HbIcon getIconFromEntry(const CaEntry& entry, const QSizeF &size)
{
    HbIcon icon;
    QString skinId(entry.iconDescription().skinId());
    if (!skinId.isEmpty()) {
        icon = HbIcon(skinId);
    }
    
    if (icon.isNull() || !(icon.size().isValid())) {
        QString fileName(entry.iconDescription().filename());
        if (!fileName.isEmpty()) {
            icon = HbIcon(fileName);
            if (fileName.contains(QChar('.'))
                    && !icon.isNull() && icon.size().isValid()) {
                icon.setSize(size);
                icon = HbIcon(QIcon(icon.pixmap()));
            }
        }
    }
    return icon;
}

/*!
 Get icon from apparc.
 \param entry const reference to CaEntry.
 \param sie const reference to icon size.
 \retval icon.
 */
LOCAL_C HbIcon getIconFromApparcL(int uidValue, const QSizeF &size)
{
    HbIcon icon;
    
    RApaLsSession apaLsSession;
    CleanupClosePushL(apaLsSession);
    User::LeaveIfError(apaLsSession.Connect());

    TUid uid = uid.Uid(uidValue);
    CApaAppServiceInfoArray* skinArray(NULL);
    TRAPD( err, skinArray =
        apaLsSession.GetAppServiceOpaqueDataLC(uid, TUid::Uid(0x2002DCF3));
        if (err == KErrNone && skinArray ) {
            TArray<TApaAppServiceInfo> tmpArray( skinArray->Array() );
            if ( tmpArray.Count() ) {
                TPtrC8 opaque(tmpArray[0].OpaqueData());
                const TPtrC16 iconName((TText16*) opaque.Ptr(),(opaque.Length()+1)>>1);
                icon = HbIcon( XQConversions:: s60DescToQString( iconName ) );
            }
        }
        CleanupStack::PopAndDestroy(skinArray);
        );
    
    if (icon.isNull() || !(icon.size().isValid())) {
        TSize iconSize(size.width(), size.height());
        CApaMaskedBitmap* apaMaskedBitmap = CApaMaskedBitmap::NewLC();
        TInt err = apaLsSession.GetAppIcon(uid,iconSize,*apaMaskedBitmap);
        
        TInt iconsCount(0);
        apaLsSession.NumberOfOwnDefinedIcons(uid, iconsCount);
        
        QPixmap pixmap;
        if ((err == KErrNone) && (iconsCount > 0)) {
            CaBitmapAdapter::fromBitmapAndMaskToPixmapL(apaMaskedBitmap,
                    apaMaskedBitmap->Mask(), pixmap);
    
            pixmap = pixmap.scaled(size.toSize(), 
                Qt::KeepAspectRatioByExpanding);
            icon = HbIcon(QIcon(pixmap));
        } else {
            HBufC* fileNameFromApparc = NULL;
            TInt err2 = apaLsSession.GetAppIcon(uid,fileNameFromApparc);
            CleanupStack::PushL(fileNameFromApparc);
            if (err2 == KErrNone) {
                QString fileName = XQConversions::s60DescToQString(
                        fileNameFromApparc->Des());
                if (fileName.endsWith(QLatin1String(".mif"),
                        Qt::CaseInsensitive) ||
                    fileName.endsWith(QLatin1String(".mbm"),
                        Qt::CaseInsensitive)) {
                    icon = HbIcon(QIcon(fileName));
                    // Icon should be valid.
                    icon.setSize(QSizeF(0.0, 0.0));
                } else {
                    icon = HbIcon(fileName);
                }
            }
            CleanupStack::PopAndDestroy(fileNameFromApparc);
        }
        CleanupStack::PopAndDestroy(apaMaskedBitmap);
    }
    CleanupStack::PopAndDestroy(&apaLsSession);
    return icon;
}

/*!
 Get default icon from theme.
 \param entry const reference to CaEntry.
 \retval icon.
 */
LOCAL_C HbIcon getDefaultIcon(const CaEntry& entry)
{
    HbIcon icon;
    
    if (entry.entryTypeName() == 
            XQConversions::s60DescToQString(KCaTypeApp)) {
        icon = HbIcon("qtg_large_application");
    } else if (entry.entryTypeName() ==
            XQConversions::s60DescToQString(KCaTypeFolder) ||
            entry.entryTypeName() == 
            XQConversions::s60DescToQString(KCaTypeCollection) ||
            entry.entryTypeName() == 
            XQConversions::s60DescToQString(KCaTypeCollectionDownload)) {
        icon = HbIcon("qtg_large_applications_user");
    } else if (entry.entryTypeName() ==
        XQConversions::s60DescToQString(KCaTypeUrl)) {
        icon = HbIcon("qtg_large_application");
    } else if (entry.entryTypeName() ==
        XQConversions::s60DescToQString(KCaTypeWidget)) {
        icon = HbIcon("qtg_large_application");
    } else if (entry.entryTypeName() ==
        XQConversions::s60DescToQString(KCaTypeTemplatedApp)) {
        icon = HbIcon("qtg_large_application");
    } else if (entry.entryTypeName() ==
        XQConversions::s60DescToQString(KCaTypePackage)) {
        QString type = entry.attribute(XQConversions::s60DescToQString(KCaAttrAppType));
        if (type == 
                XQConversions::s60DescToQString(KCaAttrAppTypeValueJava)) {
            icon = HbIcon("qtg_large_java");
        } else if (type == 
                XQConversions::s60DescToQString(KCaAttrAppTypeValueCWRT)) {
            icon = HbIcon("qtg_large_widget");
        } else {
            icon = HbIcon("qtg_large_sisx");
        }        
    }
    
    return icon;
}

/*!
 Get entry by uid.
 \param uid application uid.
 \retval entry.
 */
LOCAL_C CaEntry getEntryByUid(int uid)
{
    CaEntry entry;
    CaQuery query;
    query.setAttribute(QString("application:uid"), QString::number(uid));
    QList< QSharedPointer<CaEntry> > entries = CaService::instance()->getEntries(query);
    if (!entries.isEmpty()) {
        entry = *(entries.first());
    }
    return entry;
}

/*!
 Get icon when uid is defined.
 \param entry const reference to CaEntry.
 \param sie const reference to icon size.
 \retval icon.
 */
HbIcon CaMenuIconUtility::getApplicationIcon(int uid,
        const QSizeF &size)
{
    HbIcon icon;
    CaEntry entry;
    entry = getEntryByUid(uid);
    icon = getEntryIcon(entry, size);
    return icon;
}

/*!
 Get icon.
 \param entry const reference to CaEntry.
 \param sie const reference to icon size.
 \retval icon.
 */
HbIcon CaMenuIconUtility::getEntryIcon(const CaEntry& entry,
        const QSizeF &size)
{
    HbIcon icon;
    icon = getIconFromEntry(entry, size);
 
    if (icon.isNull() || !(icon.size().isValid())) {
        QString uidString(entry.attribute(appUidAttributeName));
        bool uidOk(false);
        int uidValue = uidString.toInt(&uidOk);
        if (!uidOk) {
            uidString = entry.iconDescription().applicationId();
            uidValue = uidString.toInt(&uidOk);
        }
        if (uidOk) {
            TRAP_IGNORE(icon = getIconFromApparcL(uidValue, size));
        }
    }
 
    if (icon.isNull() || !(icon.size().isValid())) {
        icon = getDefaultIcon(entry);
    }
 
    if (entry.entryTypeName() == XQConversions::s60DescToQString(
            KCaTypeWidget)) {
        icon.addProportionalBadge(Qt::AlignBottom | Qt::AlignRight,
		    HbIcon("qtg_small_homescreen"), QSizeF(0.5, 0.5));
    }
    return icon;
}

