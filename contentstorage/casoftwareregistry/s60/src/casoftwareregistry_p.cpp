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
 * Description:  ?Description
 *
 */
#include <HbParameterLengthLimiter>
#include <QMetaType>
#include <QScopedPointer>
#include <QStringList>

#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>
#include <xqconversions.h>
#include <driveinfo.h>

#include "casoftwareregistry.h"
#include "casoftwareregistry_p.h"

using namespace Usif;

_LIT(KConfirmMessageKey, "MIDlet-Delete-Confirm");

template <typename RClass>
struct RClassDeleter
{
    static inline void cleanup(RClass *ptr)
    {
        ptr->Close();
    }
};

typedef QScopedPointer<RSoftwareComponentRegistry, 
    RClassDeleter<RSoftwareComponentRegistry> > ScrScopedPointer;
    
typedef QScopedPointer<RFs, 
    RClassDeleter<RFs> > RFSScopedPointer;
/*!
 Constructor
 \param servicePublic Pointer to object of the public class.
 */
CaSoftwareRegistryPrivate::CaSoftwareRegistryPrivate(
    CaSoftwareRegistry *servicePublic):
    m_q(servicePublic)
{
}

/*!
 Destructor.
 */
CaSoftwareRegistryPrivate::~CaSoftwareRegistryPrivate()
{
}

/*!
 Provides details needed for uninstalling process of Java applications.
 \param[in] componentId component id of an application to be uninstalled.
 \param[out] componentName a name of the component.
 \param[out] applicationsUids a list of uids of applications in the package
      of the given component id.
 \param[out] confirmationMessage optional deletion confirmation message,
      null string means the lack of the message.
 \retval true if there is no error.
 */

bool CaSoftwareRegistryPrivate::getUninstallDetails(int componentId,
    QString &componentName,
    QStringList &applicationsUids,
    QString &confirmationMessage)
{
    TRAPD(error, getUninstallDetailsL(componentId,
        componentName,
        applicationsUids,
        confirmationMessage)
         );
    return error == KErrNone;
}

/*!
 Provides a list of uids of applications installed by the given package.
 \param[in] componentId component id of an application to be uninstalled.
 \param[out] applicationsUids a list of uids of applications in the package
      of the given component id.
 \retval true if there is no error.
 */

bool CaSoftwareRegistryPrivate::getApplicationsUids(int componentId,
    QStringList &applicationsUids)
{
    TRAPD(error, getApplicationsUidsL(componentId, applicationsUids));
    return error == KErrNone;
}

/*!
 Provides details needed for uninstalling process of Java applications
   (leaving version).
 \param[in] componentId component id of an application to be uninstalled.
 \param[out] componentName a name of the component.
 \param[out] applicationsUids a list of uids of applications in the package
      of the given component id.
 \param[out] confirmationMessage optional deletion confirmation message,
      null string means the lack of the message.
 */
void CaSoftwareRegistryPrivate::getUninstallDetailsL(int componentId,
    QString &componentName,
    QStringList &appUids,
    QString &confirmationMessage)
{
    componentName.clear();
    appUids.clear();
    confirmationMessage.clear();

    if (componentId >= 1) {
        TComponentId componentIdValue(componentId);
        RArray<TUid> appUidsArray;
        CleanupClosePushL(appUidsArray);
        CLocalizablePropertyEntry *confirmationMessageProperty = NULL;

        RSoftwareComponentRegistry softwareComponentRegistry;
        CleanupClosePushL(softwareComponentRegistry);
        User::LeaveIfError(softwareComponentRegistry.Connect());

        CComponentEntry *entry = CComponentEntry::NewLC();
        softwareComponentRegistry.GetComponentL(componentId, *entry);
        softwareComponentRegistry.GetAppUidsForComponentL(componentIdValue,
            appUidsArray);

        CPropertyEntry *confirmationProperty =
            softwareComponentRegistry.GetComponentPropertyL(componentId,
            KConfirmMessageKey);
        if (confirmationProperty &&
            confirmationProperty->PropertyType() ==
                CPropertyEntry::ELocalizedProperty) {
            confirmationMessageProperty =
                static_cast<CLocalizablePropertyEntry *>(
                    confirmationProperty);
        } else {
            delete confirmationProperty;
            confirmationProperty = NULL;
        }

        QT_TRYCATCH_LEAVING(componentName =
            XQConversions::s60DescToQString(entry->Name());
            for (TInt i = 0; i<appUidsArray.Count(); i++) {
                appUids.append(QString::number(appUidsArray[i].iUid));
            }
            if (confirmationMessageProperty) {
                confirmationMessage = XQConversions::s60DescToQString(
                    confirmationMessageProperty->StrValue());
            }
        );

        CleanupStack::PopAndDestroy(3, &appUidsArray);
    }
}

/*!
 Provides details needed for uninstalling process of Java applications
   (leaving version).
 \param[in] componentId component id of an application to be uninstalled.
 \param[out] applicationsUids a list of uids of applications in the package
      of the given component id.
 */
void CaSoftwareRegistryPrivate::getApplicationsUidsL(int componentId,
    QStringList &appUids)
{
    appUids.clear();
    if (componentId >= 1) {
        TComponentId componentIdValue(componentId);
        RArray<TUid> appUidsArray;
        CleanupClosePushL(appUidsArray);

        RSoftwareComponentRegistry softwareComponentRegistry;
        CleanupClosePushL(softwareComponentRegistry);
        User::LeaveIfError(softwareComponentRegistry.Connect());

        softwareComponentRegistry.GetAppUidsForComponentL(componentIdValue,
            appUidsArray);

        QT_TRYCATCH_LEAVING(
            for (TInt i = 0; i<appUidsArray.Count(); i++) {
                appUids.append(QString::number(appUidsArray[i].iUid));
            }
        );
        CleanupStack::PopAndDestroy(2, &appUidsArray);
    }
}

/*!
 \param componentId Component id of the entry which details are requested for.
 \return Map of component details if component id was greater than 0 or
 empty map otherwise. 
 */
CaSoftwareRegistryPrivate::DetailMap CaSoftwareRegistryPrivate::entryDetails(
    int componentId) const
{
    CaSoftwareRegistry::DetailMap result;

    if (componentId >= 1) {
        RSoftwareComponentRegistry softwareComponentRegistry;
        ScrScopedPointer scr(&softwareComponentRegistry);
        if (scr->Connect() == KErrNone) {
            
            QScopedPointer<CComponentEntry> entry;
            
            QT_TRAP_THROWING(entry.reset(CComponentEntry::NewL()));
            
            TBool resultCode = EFalse;
            TRAPD(leaveCode, resultCode = scr->GetComponentL(componentId, *entry));
            if (leaveCode == KErrNone && resultCode) {
                result = entryDetails(*entry);
            }
        }
    }
    
    return result;
}

/*!
 \param entry Software registry entry providing details.
 \return Map with details for the component represented by \entry.
 */
CaSoftwareRegistryPrivate::DetailMap CaSoftwareRegistryPrivate::entryDetails(
    const CComponentEntry& entry) const
{
    CaSoftwareRegistry::DetailMap detailMap;
    
    detailMap[CaSoftwareRegistry::componentNameKey()] = 
        XQConversions::s60DescToQString(entry.Name());
        
    detailMap[CaSoftwareRegistry::componentVersionKey()] = 
        XQConversions::s60DescToQString(entry.Version());
    
    detailMap[CaSoftwareRegistry::componentVendorKey()] = 
        XQConversions::s60DescToQString(entry.Vendor());
        
    QString drives;
    QString drv;
    TChar drive;    
    
    const TInt driveListLen(entry.InstalledDrives().Length());
    for (TInt i( 0 ); i < driveListLen; ++i) {
        if (entry.InstalledDrives()[i] != '\0') {
            
            if (!drives.isEmpty()) {
                drives = drives.append(",");
            }  
            drv = QString(QChar('A'+ i)).append(":");
            if(DriveInfo::GetDefaultDrive(
                    DriveInfo::EDefaultPhoneMemory, drive ) == KErrNone 
                    && QChar('A'+ i) == QChar(drive))
                {
                drives = drives.append(
                        HbParameterLengthLimiter("txt_applib_dialog_1_device_memory").arg(
                          QString(QChar('A'+ i)).append(":")));
                }
            else if(DriveInfo::GetDefaultDrive(
                    DriveInfo::EDefaultMassStorage, drive ) == KErrNone 
                    && QChar('A'+ i) == QChar(drive))
                {
                drives = drives.append(
                        HbParameterLengthLimiter("txt_applib_dialog_1_mass_storage").arg(
                          QString(QChar('A'+ i)).append(":")));
                }
            else if(DriveInfo::GetDefaultDrive(
                    DriveInfo::EDefaultRemovableMassStorage, drive ) == KErrNone 
                    && QChar('A'+ i) == QChar(drive))
                {
                RFs fs;
                RFSScopedPointer fSPointer(&fs);
                if (fs.Connect() == KErrNone) {
                    TInt driveNumber;
                    TVolumeInfo tv;
                    DriveInfo::GetDefaultDrive(
                            DriveInfo::EDefaultRemovableMassStorage, driveNumber );
                    qt_symbian_throwIfError(fs.Volume(tv, driveNumber));
                    if(tv.iName.Length()) { 
                        drives = drives.append(
                                HbParameterLengthLimiter("txt_applib_dialog_1_2").arg(
                                   QString(QChar('A'+ i)).append(":")).arg(
                                           XQConversions::s60DescToQString(tv.iName)));                       
                    }
                    else {
                        drives = drives.append(
                            HbParameterLengthLimiter("txt_applib_dialog_1_memory_card").arg(
                              QString(QChar('A'+ i)).append(":")));           
                    }
                }
            } else {
                drives = drives.append(QChar('A'+ i)).append(":");   
            }
        }
    }
    
    detailMap[CaSoftwareRegistry::componentDriveInfoKey()] = drives;
    

    static const TInt64 KKilo = 1024;
    static const TInt64 KMega = KKilo * KKilo;
    if(entry.ComponentSize() >= KMega) {
        detailMap[CaSoftwareRegistry::componentSizeKey()] = 
            HbParameterLengthLimiter("txt_applib_dialog_l1_mb").arg(QString().setNum(
                    static_cast<double>(entry.ComponentSize() / KMega)));
    }
    else {
        detailMap[CaSoftwareRegistry::componentSizeKey()] = 
            HbParameterLengthLimiter("txt_applib_dialog_l1_kb").arg(QString().setNum(
                    static_cast<double>(entry.ComponentSize() / KKilo)));
    }

    
    detailMap[CaSoftwareRegistry::componentTypeKey()] = 
        XQConversions::s60DescToQString(entry.SoftwareType());

    return detailMap;
}
