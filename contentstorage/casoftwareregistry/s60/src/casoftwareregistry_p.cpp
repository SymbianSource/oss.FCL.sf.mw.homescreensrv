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
#include <QtGlobal>
#include <QMetaType>
#include <QScopedPointer>
#include <QString>

#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>
#include <xqconversions.h>
#include <driveinfo.h>

#include "casoftwareregistry.h"
#include "casoftwareregistry_p.h"

using namespace Usif;

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
        QT_TRAP_THROWING(User::LeaveIfError(scr->Connect()));
        
        QScopedPointer<CComponentEntry> entry;
        
        QT_TRAP_THROWING(entry.reset(CComponentEntry::NewL()));
        QT_TRAP_THROWING(scr->GetComponentL(componentId, *entry));
        result = entryDetails(*entry);
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
    TChar drive;
    
    const TInt driveListLen(entry.InstalledDrives().Length());
    for (TInt i( 0 ); i < driveListLen; ++i) {
        if (entry.InstalledDrives()[i] != '\0') {
            
            if (!drives.isEmpty()) {
                drives = drives.append(",");
             }  
            
            if(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive ) == KErrNone 
                    && QChar('A'+ i) == QChar(drive))
                {
                drives = drives.append(QChar('A'+ i)).append(":").append(
                        " txt_applib_dialog_1_device_memory");
                }
            else if(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive ) == KErrNone 
                    && QChar('A'+ i) == QChar(drive))
                {
                drives = drives.append(QChar('A'+ i)).append(":").append(
                        " txt_applib_dialog_1_mass_storage");
                }
            else if(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRemovableMassStorage, drive ) == KErrNone 
                    && QChar('A'+ i) == QChar(drive))
                {
                RFs fs;
                RFSScopedPointer fSPointer(&fs);
                qt_symbian_throwIfError(fs.Connect());

                TInt driveNumber;
                TVolumeInfo tv;
                DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRemovableMassStorage, driveNumber );
                
                qt_symbian_throwIfError(fs.Volume(tv, driveNumber));
                if(tv.iName.Length()) {
                    drives = drives.append(QChar('A'+ i)).append(": ").append(
                                XQConversions::s60DescToQString(tv.iName));                
                }
                else {
                    drives = drives.append(QChar('A'+ i)).append(":").append(
                        " txt_applib_dialog_1_memory_card");                
                }
            }
            else {
                drives = drives.append(QChar('A'+ i)).append(":");   
            }
        }
    }
    
    detailMap[CaSoftwareRegistry::componentDriveInfoKey()] = drives;
    detailMap[CaSoftwareRegistry::componentSizeKey()].setNum(
        entry.ComponentSize() / 1024);
    
    detailMap[CaSoftwareRegistry::componentTypeKey()] = 
        XQConversions::s60DescToQString(entry.SoftwareType());

    return detailMap;
}
