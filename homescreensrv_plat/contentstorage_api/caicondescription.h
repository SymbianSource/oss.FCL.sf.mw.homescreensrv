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
 * Description: caicondescription.h
 *
 */

#ifndef CAICONDESCRIPTION_H
#define CAICONDESCRIPTION_H

#include <QSharedDataPointer>

#include "caclient_global.h"

class CaObjectAdapter;

class CaIconDescriptionPrivate;

class CACLIENT_EXPORT CaIconDescription
{

public:

    CaIconDescription();
    CaIconDescription(const CaIconDescription &iconDescription);
    ~CaIconDescription();

    CaIconDescription &operator=(const CaIconDescription &iconDescription);

    int id() const;

    QString filename() const;
    void setFilename(const QString &filename);

    int bitmapId() const;
    void setBitmapId(int id);

    int maskId() const;
    void setMaskId(int id);

    int skinMajorId() const;
    void setSkinMajorId(int id);
    int skinMinorId() const;
    void setSkinMinorId(int id);

private:

    void setId(int id);

private:
    /**
     * Pointer to a private implementation.
     */
    QSharedDataPointer<CaIconDescriptionPrivate> m_d;

    friend class CaIconDescriptionPrivate;
    friend class ::CaObjectAdapter;
};

#endif // CAICONDESCRIPTION_H
