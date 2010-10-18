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
#include <QObject>
#include <QDateTime>
#include <QVariantHash>

#include <xqconversions.h>

#include "tsservice.h"
#include "tsmodelobserver.h"
#include "tsserviceobserver.h"
#include "tsresourcemanager.h"


// -----------------------------------------------------------------------------
LOCAL_C QVariantHash value( QObject *aModel, TInt aOffset )
    {
    QList<QVariantHash> items;
    QMetaObject::invokeMethod( aModel, 
                               "taskList", 
                               Q_RETURN_ARG(QList<QVariantHash>, 
                               items ) );
    return (items.count() > aOffset) ? items.at(aOffset) : QVariantHash();
    }

// -----------------------------------------------------------------------------
CTsService* CTsService::NewLC(MTsResourceManager& resources, QObject* model)
    {
    CTsService *self = new (ELeave)CTsService();
    CleanupStack::PushL(self);
    self->ConstructL(resources, model);
    return self;
    }

// -----------------------------------------------------------------------------
CTsService::CTsService()
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
void CTsService::ConstructL( MTsResourceManager& resources, QObject* model )
    {
    iModel = model;
    User::LeaveIfNull(iModel);
    // @todo: add checking for all necessary methods
    
    iServiceObserver = new TsServiceObserver(*this);
    User::LeaveIfNull(iServiceObserver);
    QObject::connect(iModel, 
                     SIGNAL(dataChanged()),
                     iServiceObserver,
                     SLOT(dataChanged()));
    QMetaObject::invokeMethod(iModel,
                              "setResources",
                              Q_ARG(MTsResourceManager&, resources));
    }

// -----------------------------------------------------------------------------
CTsService::~CTsService()
    {
    delete iModel;
    delete iServiceObserver;
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
void CTsService::DataChanged()
    {
    if(0 != iModelObserver) 
        {
        iModelObserver->DataChanged(*this);
        }
    }
// -----------------------------------------------------------------------------
TInt CTsService::Count() const
    {
    QList<QVariantHash> items;
    QMetaObject::invokeMethod(iModel, 
                              "taskList", 
                              Q_RETURN_ARG(QList<QVariantHash>, items));
    return items.count();
    }

// -----------------------------------------------------------------------------
void CTsService::SetObserver( MTsModelObserver *aObserver )
    {
    iModelObserver = aObserver;
    }

// -----------------------------------------------------------------------------
const TDesC& CTsService::DisplayName( TInt aOffset ) const
    {
    return StringValue( aOffset, "TaskName" );
    }

// -----------------------------------------------------------------------------
TInt CTsService::IconHandle( TInt aOffset ) const
    {
    return IntValue( aOffset, "TaskScreenshot" );
    }

// -----------------------------------------------------------------------------
TTime CTsService::Timestamp( TInt aOffset ) const
    {
    return TimeValue( aOffset, "TaskTimestamp" );
    }

// -----------------------------------------------------------------------------
TTime CTsService::TimestampUpdate(TInt offset) const
{
    return TimeValue(offset, "TaskUpdateTimestamp");
}

TTsEntryKey CTsService::Key( TInt aOffset ) const
    {
    return TTsEntryKey( IntValue( aOffset, "TaskId" ), 
                        reinterpret_cast<TInt>( this ) );
    }

// -----------------------------------------------------------------------------
TBool CTsService::IsActive( TInt aOffset ) const
    {
    return IntValue( aOffset, "TaskIsRunning" );
    }

// -----------------------------------------------------------------------------
TBool CTsService::IsClosable( TInt aOffset ) const
    {
    return IntValue( aOffset, "TaskCanBeClosed" );
    }

// -----------------------------------------------------------------------------
TBool CTsService::IsMandatory( TInt aOffset ) const
    {
    const QVariantHash values(value(iModel, aOffset));
    const QString key("TaskIsMandatory");
    return values.contains(key) ? values.value(key).toInt() : ETrue;
    }

// -----------------------------------------------------------------------------
TBool CTsService::Close( TTsEntryKey aKey ) const
    {
    bool result(false);
    QMetaObject::invokeMethod( iModel, 
                               "closeTask", 
                               Q_RETURN_ARG(bool, result), 
                               Q_ARG(QVariant, aKey.Key() ) );
    return result;
    }

// -----------------------------------------------------------------------------
TBool CTsService::Launch( TTsEntryKey aKey ) const
    {
    bool result(false);
    QMetaObject::invokeMethod( iModel, 
                               "openTask", 
                               Q_RETURN_ARG(bool, result), 
                               Q_ARG(QVariant, aKey.Key() ) );
    return result;
    }

// -----------------------------------------------------------------------------
TInt CTsService::IntValue( TInt aOffset, const char* aKey) const
    {
    return value(iModel, aOffset).value( aKey ).toInt();
    }

// -----------------------------------------------------------------------------
TTime CTsService::TimeValue(TInt aOffset, const char* aKey) const
    {
    // Conversion between TTime which counts from year 0, and QDateTime which uses unix epoch (1st Jan 1970)
    QDateTime timestamp = value( iModel, aOffset ).value( aKey ).toDateTime();
    
    return TTime( _L( "19700000:" ) ) + TTimeIntervalSeconds( timestamp.toTime_t() ) +
                         TTimeIntervalMicroSeconds( timestamp.time().msec() * 1000 );
    }

// -----------------------------------------------------------------------------
const TDesC& CTsService::StringValue( TInt aOffset, const char* aKey ) const
    {
    delete iBuffer;
    iBuffer = 0;
    
    const QVariantHash item(value(iModel, aOffset));
    try {
        iBuffer = XQConversions::qStringToS60Desc(item.value(aKey).toString());
    } catch(...) {
    }
    return iBuffer ? *iBuffer : KNullDesC();
    }
