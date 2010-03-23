/*
 * Copyright (c)2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <exception>
#include <e32base.h>
#include <badesca.h>
#include <AknIconUtils.h>
#include <fbs.h>
#include <bitdev.h>
#include <QPixmap>
#include <HbIcon>
#include <QBitmap>
#include <QDebug>

#include "cadef.h"
#include "caobjectadapter.h"
#include "cainnerentry.h"
#include "caentry_p.h"
#include "cainnerquery.h"
#include "caquery.h"
#include "camenuiconutility.h"
#include "canotifierfilter.h"
#include "cainnernotifierfilter.h"
static  QImage::Format TDisplayMode2Format(TDisplayMode mode)
{
    QImage::Format format;
    switch(mode) {
    case EGray2:
        format = QImage::Format_MonoLSB;
        break;
    case EColor256:
    case EGray256:
        format = QImage::Format_Indexed8;
        break;
    case EColor4K:
        format = QImage::Format_RGB444;
        break;
    case EColor64K:
        format = QImage::Format_RGB16;
        break;
    case EColor16M:
        format = QImage::Format_RGB888;
        break;
    case EColor16MU:
        format = QImage::Format_RGB32;
        break;
    case EColor16MA:
        format = QImage::Format_ARGB32;
        break;
    case EColor16MAP:
        format = QImage::Format_ARGB32_Premultiplied;
        break;
    default:
        format = QImage::Format_Invalid;
        break;
    }
    return format;
}

QPixmap fromSymbianCFbsBitmap(CFbsBitmap* aBitmap)
    {
    aBitmap->BeginDataAccess();
    uchar *data = (uchar*)aBitmap->DataAddress();
    TSize size = aBitmap->SizeInPixels();
    TDisplayMode displayMode = aBitmap->DisplayMode();    
    
    // QImage format must match to bitmap format
    QImage image(data, size.iWidth, size.iHeight, TDisplayMode2Format(displayMode));
    aBitmap->EndDataAccess();
      
    // No data copying happens because image format matches native OpenVG format.
    // So QPixmap actually points to CFbsBitmap data.
    return QPixmap::fromImage(image);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CaObjectAdapter::convertL(const CaEntry &fromEntry,
    CCaInnerEntry &toEntry)
{
    toEntry.SetId(fromEntry.id());

    toEntry.SetTextL(convertToDescriptor(fromEntry.text()));

    toEntry.SetDescriptionL(convertToDescriptor(fromEntry.description()));

    toEntry.SetEntryTypeNameL(
        convertToDescriptor(fromEntry.entryTypeName()));

    toEntry.SetFlags(static_cast<TUint> (fromEntry.flags()));

    toEntry.SetRole(static_cast<TUint> (fromEntry.role()));

    const CaIconDescription fromIconDescription =
        fromEntry.iconDescription();

    toEntry.SetIconDataL(
        static_cast<TInt>(fromIconDescription.bitmapId()),
        static_cast<TInt>(fromIconDescription.maskId()),
        static_cast<TInt>(fromIconDescription.skinMajorId()),
        static_cast<TInt>(fromIconDescription.skinMinorId()),
        convertToDescriptor(fromIconDescription.filename()));

    const QMap<QString, QString> attributesMap = fromEntry.attributes();

    foreach (QString key, attributesMap.keys()) {
        if (key == applicationUidAttributeName()) {
            const TInt32 uid = attributesMap.value(key).toInt();
            toEntry.SetUid(uid);
        }
        else {
            toEntry.AddAttributeL(
                convertToDescriptor(key),
                convertToDescriptor(attributesMap.value(key)));
        }
    }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convertL(const CaQuery &fromQuery,
    CCaInnerQuery &toQuery)
{
    toQuery.SetParentId(fromQuery.parentId());

    toQuery.SetFlagsOn(static_cast<TUint>(fromQuery.flagsOn()));

    toQuery.SetFlagsOff(static_cast<TUint>(fromQuery.flagsOff()));

    toQuery.SetRole(static_cast<TInt>(fromQuery.entryRoles()));

    toQuery.SetCount(fromQuery.count());

    const QStringList list = fromQuery.entryTypeNames();

    CDesC16ArrayFlat *sourceList =
        new (ELeave)CDesC16ArrayFlat(KDefaultGranularity);
    CleanupStack::PushL(sourceList);

    foreach (const QString str, list) {
        sourceList->AppendL(convertToDescriptor(str));
    }

    toQuery.SetEntryTypeNames(sourceList);

    CleanupStack::Pop(sourceList);

    // mapping sort enums
    SortAttribute sortAttr;
    Qt::SortOrder sortOrder;
    fromQuery.getSort(sortAttr, sortOrder);
    toQuery.SetSort(CaObjectAdapter::getSortCode(sortAttr, sortOrder));
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convert(const CCaInnerEntry &fromEntry,
    CaEntry &toEntry)
{
    toEntry.setId(fromEntry.GetId());

    toEntry.setText(convertToString(fromEntry.GetText()));

    toEntry.setDescription(convertToString(fromEntry.GetDescription()));
    toEntry.setEntryTypeName(convertToString(fromEntry.GetEntryTypeName()));

    toEntry.setFlags(static_cast<EntryFlag>(fromEntry.GetFlags()));

    // take care of converting icon attributes
    const CCaInnerEntry::TIconAttributes &icon = fromEntry.GetIcon();

    CaIconDescription iconDescription;
    iconDescription.setId(icon.iId);
    iconDescription.setBitmapId(icon.iBitmapId);
    iconDescription.setMaskId(icon.iMaskId);
    iconDescription.setSkinMajorId(icon.iSkinMajorId);
    iconDescription.setSkinMinorId(icon.iSkinMinorId);
    iconDescription.setFilename(convertToString(icon.iFileName));

    toEntry.setIconDescription(iconDescription);

    const RCaEntryAttrArray &attributesArray = fromEntry.GetAttributes();

    if (toEntry.entryTypeName() == applicationEntryName()
        || toEntry.entryTypeName() == widgetEntryName()) {
        toEntry.setAttribute(applicationUidAttributeName(),
                QString::number(fromEntry.GetUid()));
    }

    const int attributesArrayCount = attributesArray.Count();

    for (int i = 0; i < attributesArrayCount; ++i) {
        const CCaEntryAttribute *const attribute = attributesArray[i];
        toEntry.setAttribute(convertToString(attribute->Name()),
            convertToString(attribute->Value()));
    }
}

//----------------------------------------------------------------------------
// !!! Warning: returns wrapper to internal QString representation which may
// get invalid before returned TPtrC object gets out of scope
//----------------------------------------------------------------------------
const TPtrC CaObjectAdapter::convertToDescriptor(const QString &string)
{
    const TPtrC result(reinterpret_cast<const TUint16*> (string.utf16()));
    return result;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
QString CaObjectAdapter::convertToString(const TDesC &des)
{
    return QString::fromUtf16(des.Ptr(), des.Length());
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convertL(const CaNotifierFilter &notifierFilter,
    CCaInnerNotifierFilter &source)
{
    source.SetParentId(static_cast<TInt>(notifierFilter.getParentId()));

    RArray<TInt> idsList;
    CleanupClosePushL(idsList);
    CaObjectAdapter::convertL(notifierFilter.getIds(), idsList);
    source.SetIdsL(idsList);

    CleanupStack::PopAndDestroy(&idsList);

    switch (notifierFilter.getEntryRole()) {
    case static_cast<TUint>(ItemEntryRole):
        source.SetEntryRole(EItemEntryRole);
        break;
    case static_cast<TUint>(GroupEntryRole):
        source.SetEntryRole(EGroupEntryRole);
        break;
    default:
        source.SetEntryRole(ENoneEntryRole);
        break;
    }
    CDesC16ArrayFlat *typeNames = new (ELeave) CDesC16ArrayFlat(
        KDefaultGranularity);
    CleanupStack::PushL(typeNames);

    const QStringList stringList = notifierFilter.getTypeNames();

    foreach(QString str, stringList) {
        typeNames->AppendL(convertToDescriptor(str));
    }

    source.SetTypeNames(typeNames);

    CleanupStack::Pop(typeNames);

}
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convertL(
    const RPointerArray<CCaInnerEntry> &fromEntriesArray,
    QList<CaEntry*> &toEntryList)
{
    for (int i = 0; i < fromEntriesArray.Count(); ++i) {
        CaEntry *const toEntry = new CaEntry(
            static_cast<EntryRole>(fromEntriesArray[i]->GetRole()));

        CaObjectAdapter::convert(*fromEntriesArray[i], *toEntry);

        toEntryList << toEntry;
    }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convertL(const QList<int> &entryIdList,
    RArray<TInt> &source)
{
    foreach(int id, entryIdList)
        {
            source.AppendL(static_cast<TInt>(id));
        }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convertL(const RArray<TInt> &innerEntryIdList,
    QList<int> &idList)
{
    for (TInt i = 0; i < innerEntryIdList.Count(); ++i) {
        idList.append(innerEntryIdList[i]);
    }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
HbIcon CaObjectAdapter::makeIcon(const CaEntry &entry, const QSize &size)
{
    HbIcon icon;
    TRAPD(leaveCode, icon = makeIconL(entry, size));

    USE_QDEBUG_IF(leaveCode) << "CaObjectAdapter::makeIcon leaveCode:"
        << leaveCode;

    return icon;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
ErrorCode CaObjectAdapter::convertErrorCode(int internalErrorCode)
{
    ErrorCode error(NoErrorCode);
    switch (internalErrorCode) {
    case KErrNone:
        error = NoErrorCode;
        break;
    case KErrNotFound:
        error = NotFoundErrorCode;
        break;
    case KErrNoMemory:
        error = OutOfMemoryErrorCode;
        break;
    case KErrArgument:
        error = BadArgumentErrorCode;
        break;
    default:
        error = UnknownErrorCode;
        break;
    }
    return error;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
const QString &CaObjectAdapter::applicationUidAttributeName()
{
    const static QString name("application:uid");
    return name;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
const QString &CaObjectAdapter::applicationEntryName()
{
    const static QString name("application");
    return name;
}

//----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString &CaObjectAdapter::widgetEntryName()
{
    const static QString name("widget");
    return name;
}

// -----------------------------------------------------------------------------
// copying compressed bitmap
//----------------------------------------------------------------------------
CFbsBitmap *CaObjectAdapter::copyBitmapLC(CFbsBitmap *input)
{
    CFbsBitmap *bmp = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bmp);
    bmp->Create(input->SizeInPixels(), input->DisplayMode());

    CFbsBitmapDevice *bitmapDevice = CFbsBitmapDevice::NewL(bmp);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc *bmpGc;
    bitmapDevice->CreateContext(bmpGc);
    bmpGc->BitBlt(TPoint(0,0), input);
    delete bmpGc;
    CleanupStack::PopAndDestroy(bitmapDevice);
    return bmp;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
HbIcon CaObjectAdapter::makeIconL(const CaEntry &entry, const QSize &size)
{
    HbIcon icon;
    CCaInnerEntry *innerEntry = CCaInnerEntry::NewLC();
    CaObjectAdapter::convertL(entry, *innerEntry);
    QString filename(entry.iconDescription().filename());
    if (!filename.isEmpty()) {
        icon = HbIcon(filename);
    }
    //try to load symbian icon from multi-bitmap (mbm, mbg)
    if (icon.isNull() || !(icon.size().isValid())) {
        CAknIcon *aknIcon = CaMenuIconUtility::GetItemIcon(*innerEntry);
        QPixmap pixmap;
        if (aknIcon) {
            CleanupStack::PushL(aknIcon);

            //need to disable compression to properly convert the bitmap
            AknIconUtils::DisableCompression(aknIcon->Bitmap());
            AknIconUtils::SetSize(aknIcon->Bitmap(), TSize(size.width(),
                size.height()), EAspectRatioPreservedAndUnusedSpaceRemoved);
            if (aknIcon->Bitmap()->Header().iCompression
                == ENoBitmapCompression) {
                pixmap = fromSymbianCFbsBitmap(aknIcon->Bitmap());
                QPixmap mask = fromSymbianCFbsBitmap(
                    aknIcon->Mask());
                pixmap.setAlphaChannel(mask);
            }
            else // we need special handling for icons in 9.2 (NGA)
            { // let's hope that in future it will be in QT code
                CFbsBitmap *temp(NULL);
                temp = copyBitmapLC(aknIcon->Bitmap());
                pixmap = fromSymbianCFbsBitmap(temp);
                CleanupStack::PopAndDestroy();
                temp = copyBitmapLC(aknIcon->Mask());
                QPixmap mask = fromSymbianCFbsBitmap(temp);
                CleanupStack::PopAndDestroy();
                pixmap.setAlphaChannel(mask);
            }
            pixmap = pixmap.scaled(size, Qt::KeepAspectRatioByExpanding);
            CleanupStack::PopAndDestroy(aknIcon);
            icon = HbIcon(QIcon(pixmap));
        }
    }
    CleanupStack::PopAndDestroy(innerEntry);
    return icon;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convert(
    CaNotifierPrivate::NotifierType src,
    CCaInnerNotifierFilter::TNotifierType &dest)
{
    switch(src){
    case CaNotifierPrivate::EntryChangedWithIdNotifierType:
        dest = CCaInnerNotifierFilter::EEntryChangedWithId;
        break;
    case CaNotifierPrivate::EntryChangedWithEntryNotifierType:
        dest = CCaInnerNotifierFilter::EEntryChangedWithEntry;
        break;
    case CaNotifierPrivate::EntryTouchedNotifierType:
        dest = CCaInnerNotifierFilter::EEntryTouched;
        break;
    case CaNotifierPrivate::GroupContentChangedNotifierType:
        dest = CCaInnerNotifierFilter::EGroupContentChanged;
        break;
    }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::convert(
    TChangeType src, ChangeType &dest)
{
    switch(src){
    case EAddChangeType:
        dest = AddChangeType;
        break;
    case ERemoveChangeType:
        dest = RemoveChangeType;
        break;
    case EUpdateChangeType:
        dest = UpdateChangeType;
        break;
    }
}

CCaInnerQuery::TSortAttribute CaObjectAdapter::getSortCode(
    SortAttribute sortAttribute, Qt::SortOrder sortOrder)
{
    CCaInnerQuery::TSortAttribute sortCode(CCaInnerQuery::Default);

    switch (sortAttribute) {
    case NameSortAttribute:
    {
        sortCode = (sortOrder == Qt::AscendingOrder) ? CCaInnerQuery::Name
            : CCaInnerQuery::NameDesc;
        break;
    }
    case CreatedTimestampSortAttribute:
    {
        sortCode = (sortOrder == Qt::AscendingOrder)
            ? CCaInnerQuery::CreatedTimestamp
            : CCaInnerQuery::CreatedTimestampDesc;
        break;
    }
    case MostUsedSortAttribute:
    {
        sortCode = (sortOrder == Qt::AscendingOrder)
            ? CCaInnerQuery::MostUsed : CCaInnerQuery::MostUsedDesc;
        break;
    }
    case LastUsedSortAttribute:
    {
        sortCode = (sortOrder == Qt::AscendingOrder)
            ? CCaInnerQuery::LastUsed : CCaInnerQuery::LastUsedDesc;
        break;
    }
    case DefaultSortAttribute:
    {
        sortCode = (sortOrder == Qt::AscendingOrder)
            ? CCaInnerQuery::Default : CCaInnerQuery::DefaultDesc;
        break;
    }
    }

    return sortCode;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::setId(CaEntry &entry,
    int id)
{
    entry.setId(id);
}
