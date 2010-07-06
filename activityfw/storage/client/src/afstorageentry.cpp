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

#include <xqconversions.h>

#include "afstorageentry.h"
#include "afentry.h"
#include "afserializer.h"
#include "afstorageglobals.h"
// -----------------------------------------------------------------------------
/**
 * Constructor. 
 * Initialize object with preallocated symbian specyfic data structure. Function doesn't copy content
 * @param data - pointer to symbian specyfic data structure
 */
AfStorageEntry::AfStorageEntry(QSharedPointer<CAfEntry> data)
:
mData(data)
{}

// -----------------------------------------------------------------------------
/**
* Constructor
* Initialize object with preallocated data structure. Function doesn't copy content
* @param data - activity data structure
*/
AfStorageEntry::AfStorageEntry(const AfStorageEntry &data)
:
    mData(data.mData)
{}

// -----------------------------------------------------------------------------
/**
 * Constructor
 * Initialize object with data. Function do deep copy of data
 * @param appId - unique application identifier
 * @param actId - activity identifier
 * @param privateData - private part of activity data
 * @param publicData - public part of activity data
 */
AfStorageEntry::AfStorageEntry(int appId, 
                               const QString &actId, 
                               const QVariant &privateData, 
                               const QVariantHash& publicData)
:
    mData(0)
{
    int flags(0);
    QVariantHash privData;
    privData.insert(ActivityDataKeyword, privateData);
    RBuf8 privateBuff, publicBuff;
    CleanupClosePushL(privateBuff);
    CleanupClosePushL(publicBuff);
    privateBuff << privData;
    publicBuff << publicData;
    
    if (publicData.end() != publicData.find(ActivityPersistence) && 
        publicData[ActivityPersistence].toBool()) {
        flags |= CAfEntry::Persistent;
    }
    
    if (publicData.end() != publicData.find(ActivityVisibility) && 
        !publicData[ActivityVisibility].toBool()) {
        flags |= CAfEntry::Invisible;
    }
    
    HBufC *actBuff = XQConversions::qStringToS60Desc(actId);
    CleanupStack::PushL(actBuff);
    mData = QSharedPointer<CAfEntry>(CAfEntry::NewL(flags, 
                                                    appId, 
                                                    *actBuff, 
                                                    KNullDesC, 
                                                    privateBuff, 
                                                    publicBuff));
    CleanupStack::PopAndDestroy(actBuff);
    CleanupStack::PopAndDestroy(&publicBuff);
    CleanupStack::PopAndDestroy(&privateBuff);
}

// -----------------------------------------------------------------------------
/**
 * Retrieve S60 structure representation
 */
const CAfEntry* AfStorageEntry::data() const
{
    return mData.data();
}
 

// -----------------------------------------------------------------------------
/**
 * Retrieve activity flags
 * @return activity flags
 */
int AfStorageEntry::flags() const
{
    return mData->Flags();
}

// -----------------------------------------------------------------------------
/**
 * Retrieve application unique identifier
 * @return application identifier
 */
int AfStorageEntry::applicationId() const
{
    return mData->ApplicationId();
}

// -----------------------------------------------------------------------------
/**
 * Retrieve activity identifier. Identyfier is unique for each application 
 * @return activity identifier
 */
QString AfStorageEntry::activityId() const
{
    return XQConversions::s60DescToQString(mData->ActivityId());
}

// -----------------------------------------------------------------------------
/**
 * Retrieve path to activity thumbanil path 
 * @return thumbail loacation path
 */
QString AfStorageEntry::imageSrc() const
{
    return XQConversions::s60DescToQString(mData->ImageSrc());
}

// -----------------------------------------------------------------------------
/**
 * Retrieve public part of activity data 
 * @return public activity data
 */
QVariantHash AfStorageEntry::publicData() const
{
    QVariantHash deserializedData;
    deserializedData << mData->Data(CAfEntry::Public);
    deserializedData.insert(ActivityApplicationKeyword, applicationId());
    deserializedData.insert(ActivityActivityKeyword, activityId());
    deserializedData.insert(ActivityScreenshotKeyword, imageSrc());
    deserializedData.insert(ActivityPersistence, (flags() & CAfEntry::Persistent) ? true : false);
    deserializedData.insert(ActivityVisibility, (flags() & CAfEntry::Invisible) ? false : true);
    return deserializedData;
}

// -----------------------------------------------------------------------------
/**
 * Retrieve private part of activity data 
 * @return private activity data
 */
QVariant AfStorageEntry::privateData() const
{
    QVariantHash deserializedData;
    deserializedData << mData->Data(CAfEntry::Private);
    return deserializedData[ActivityDataKeyword];
}
