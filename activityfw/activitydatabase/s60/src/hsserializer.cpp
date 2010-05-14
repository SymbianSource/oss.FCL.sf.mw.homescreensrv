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
#include "hsserializer.h"
#include <s32mem.h>
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
RBuf8 &operator <<(RBuf8 &dst, const QPixmap &src)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    QT_TRYCATCH_LEAVING(stream << src);
    const int dataLength(buffer.length());
    const unsigned char *dataPtr(reinterpret_cast<const unsigned char *>(buffer.constData()));
    if (dst.MaxLength() < dataLength) {
        dst.ReAllocL(dataLength);
    }
    dst.Copy(dataPtr, dataLength);
    return dst;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QPixmap &operator <<(QPixmap &dst, const TDesC8 &src)
{
    QByteArray buffer(QByteArray::fromRawData(reinterpret_cast<const char *>(src.Ptr()),
                                              src.Length()) );

    QDataStream stream(&buffer, QIODevice::ReadOnly);
    QT_TRYCATCH_LEAVING(stream >> dst);
    return dst;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
RBuf8 &operator <<(RBuf8 &dst, const QVariantHash &src)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    QT_TRYCATCH_LEAVING(stream << src);
    const int dataLength(buffer.length());
    const unsigned char *dataPtr(reinterpret_cast<const unsigned char *>(buffer.constData()));
    if (dst.MaxLength() < dataLength) {
        dst.ReAllocL(dataLength);
    }
    dst.Copy(dataPtr, dataLength);
    return dst;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariantHash &operator <<(QVariantHash &dst, const TDesC8 &src)
{
    QByteArray buffer(QByteArray::fromRawData(reinterpret_cast<const char *>(src.Ptr()),
                                               src.Length()) );

    QDataStream stream(&buffer, QIODevice::ReadOnly);
    QT_TRYCATCH_LEAVING(stream >> dst);
    return dst;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
RBuf8 &operator <<(RBuf8 &dst, const QList<QVariantHash>& src)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    QT_TRYCATCH_LEAVING(stream << src);

    if (dst.MaxLength() < buffer.length()) {
        dst.ReAllocL(buffer.length());
    }
    dst.Copy(reinterpret_cast<const TUint8 *>(buffer.data()), buffer.length());
    return dst;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QList<QVariantHash>& operator <<(QList<QVariantHash>& dst, const TDesC8 &src)
{
    dst.clear();
    
    QVariantHash item;
    TRAP_IGNORE(
    RDesReadStream srcStream(src);
    int numOfItems(srcStream.ReadInt32L());
    int bufferSize;
    
    RBuf8 tmpBuff;
    CleanupClosePushL(tmpBuff);
    for (int iter(0); iter < numOfItems; ++iter) {
        bufferSize = srcStream.ReadInt32L();
        if (tmpBuff.MaxLength() < bufferSize) {
            tmpBuff.ReAllocL(bufferSize);
        }
        item.clear();
        if (0 < bufferSize) {
            srcStream.ReadL(tmpBuff, bufferSize);
            item << tmpBuff;
        }
        dst.append(item);
    }
    CleanupStack::PopAndDestroy(&tmpBuff));
    return dst;
}
