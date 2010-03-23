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
 * Description: caicondescription_p.h
 *
 */

#ifndef CAICONDESCRIPTION_PRIVATE_H
#define CAICONDESCRIPTION_PRIVATE_H

#include <qstring.h>
#include <QSharedData>

class CaIconDescription;

class CaIconDescriptionPrivate: public QSharedData
{
public:

    explicit CaIconDescriptionPrivate(
        CaIconDescription *iconDescriptionPublic);
    ~CaIconDescriptionPrivate();

    int id() const;
    void setId(int id);

    QString filename() const;
    void setFileName(const QString& fileName);

    int bitmapId() const;
    void setBitmapId(int bitmapId);

    int maskId() const;
    void setMaskId(int maskId);

    int skinMajorId() const;
    void setSkinMajorId(int id);
    int skinMinorId() const;
    void setSkinMinorId(int id);

private:

    /*!
     * Points to the CaEntry instance that uses this private implementation.
     */
    CaIconDescription * const m_q;

    /*!
     *
     */
    int mId;

    /*!
     *
     */
    QString mFilename;

    /*!
     *
     */
    int mBitmapId;

    /*!
     *
     */
    int mMaskId;

    /*!
     *
     */
    int mSkinMajorId;

    /*!
     *
     */
    int mSkinMinorId;

};

#endif //CAICONDESCRIPTION_PRIVATE_H
