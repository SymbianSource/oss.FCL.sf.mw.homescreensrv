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
 * Description: caicondescription.cpp
 *
 */

#include <QDebug>

#include "caicondescription.h"
#include "caicondescription_p.h"


// ======== MEMBER FUNCTIONS ========

/*!
 \class CaIconDescription
 \brief This class describes entry's icon.

 \example
 \code
 QSharedPointer<CaService> service = CaService::instance();
 CaEntry entry;
 entry.setText("Text");
 entry.setEntryTypeName("TypeName");
 CaEntry * resultEntry = service->createEntry(entry);
 ...
 CaIconDescription iconDescription;
 iconDescription.setFileName( "z:/path/iconFileName.extension" );
 iconDescription.setBitmapId( 1234 );
 iconDescription.setMaskId( 1235 );
 iconDescription.setSkinMajorId( 987654 );
 iconDescription.setSkinMinorId( 654987 );

 resultEntry->setIconDescription( iconDescription );

 CaIconDescription entryIconDescription;
 entryIconDescription = resultEntry->iconDescription();

 ASSERT( iconDescription.fileName() == entryIconDescription.fileName() );
 ASSERT( iconDescription.bitmapId() == entryIconDescription.bitmapId() );
 ASSERT( iconDescription.maskId() == entryIconDescription.maskId() );
 ASSERT( iconDescription.skinMajorId() == entryIconDescription.skinMajorId() );
 ASSERT( iconDescription.skinMinorId() == entryIconDescription.skinMinorId() );

 delete resultEntry;
 \endcode

 */

/*!
 Default constructor.
 */
CaIconDescription::CaIconDescription() :
    m_d(new CaIconDescriptionPrivate(this))
{

}

/*!
 Copy constructor.
 \param const reference to CaIconDescription.
 */
CaIconDescription::CaIconDescription(
    const CaIconDescription &iconDescription) :
    m_d(iconDescription.m_d)
{

}

/*!
 Destructor.
 */
CaIconDescription::~CaIconDescription()
{

}

/*!
 Copy assignment operator.
 \param iconDescription const reference to CaIconDescription.
 \retval reference to CaIconDescription.
 */
CaIconDescription &CaIconDescription::operator=(
    const CaIconDescription &iconDescription)
{
    if (m_d != iconDescription.m_d) {
        m_d = iconDescription.m_d;
    }
    return *this;
}

/*!
 Returns icon id.
 \retval icon id.
 */
int CaIconDescription::id() const
{
    return m_d->id();
}

/*!
 Returns icon file name.
 \retval file name.
 */
QString CaIconDescription::filename() const
{
    return m_d->filename();
}

/*!
 Sets icon file name.
 \param QString with file name.
 */
void CaIconDescription::setFilename(const QString &filename)
{
    m_d->setFileName(filename);
}

/*!
 Returns bitmap id.
 \retval bitmap id.
 */
int CaIconDescription::bitmapId() const
{
    return m_d->bitmapId();
}

/*!
 Sets bitmap id
 \param bitmap id
 */
void CaIconDescription::setBitmapId(int id)
{
    m_d->setBitmapId(id);
}

/*!
 Returns icon mask id
 \retval icon mask id
 */
int CaIconDescription::maskId() const
{
    return m_d->maskId();
}

/*!
 Sets icon mask id
 \param icon mask id
 */
void CaIconDescription::setMaskId(int id)
{
    m_d->setMaskId(id);
}

/*!
 Returns skin major id
 \retval skin major id
 */
int CaIconDescription::skinMajorId() const
{
    return m_d->skinMajorId();
}

/*!
 Sets skin major id
 \param skin major id
 */
void CaIconDescription::setSkinMajorId(int id)
{
    m_d->setSkinMajorId(id);
}

/*!
 Returns skin minor id
 \retval skin minor id
 */
int CaIconDescription::skinMinorId() const
{
    return m_d->skinMinorId();
}

/*!
 Sets skin minor id
 \param skin minor id
 */
void CaIconDescription::setSkinMinorId(int id)
{
    m_d->setSkinMinorId(id);
}

/*!
 Sets icon id
 \param id
 */
void CaIconDescription::setId(int id)
{
    m_d->setId(id);
}

/*!
 Construcor.
 \param iconDescriptionPublic pointer to CaIconDescription.
 */
CaIconDescriptionPrivate::CaIconDescriptionPrivate(
    CaIconDescription *iconDescriptionPublic) :
    m_q(iconDescriptionPublic), mBitmapId(0), mMaskId(0), mSkinMajorId(0),
    mSkinMinorId(0)
{
}

/*!
 Destructor.
 */
CaIconDescriptionPrivate::~CaIconDescriptionPrivate()
{
}

/*!
 Returns icon id.
 \retval icon id.
 */
int CaIconDescriptionPrivate::id() const
{
    return mId;
}

/*!
 Returns file name.
 \retval file name.
 */
QString CaIconDescriptionPrivate::filename() const
{
    return mFilename;
}

/*!
 Sets a file name.
 \param fileName file name
 */
void CaIconDescriptionPrivate::setFileName(const QString &fileName)
{
    mFilename = fileName;
}

/*!
 Returns bitmap id.
 \retval bitmap id.
 */
int CaIconDescriptionPrivate::bitmapId() const
{
    return mBitmapId;
}

/*!
 Sets bitmap id.
 \param bitmap id.
 */
void CaIconDescriptionPrivate::setBitmapId(int bitmapId)
{
    mBitmapId = bitmapId;
}

/*!
 Returns icon mask id.
 \retval icon mask id.
 */
int CaIconDescriptionPrivate::maskId() const
{
    return mMaskId;
}

/*!
 Sets icon mask id.
 \param maskId icon mask id.
 */
void CaIconDescriptionPrivate::setMaskId(int maskId)
{
    mMaskId = maskId;
}

/*!
 Returns skin major id.
 \retval skin major id.
 */
int CaIconDescriptionPrivate::skinMajorId() const
{
    return mSkinMajorId;
}

/*!
 Sets skin major id.
 \param id skin major id.
 */
void CaIconDescriptionPrivate::setSkinMajorId(int id)
{
    mSkinMajorId = id;
}

/*!
 Returns skin minor id.
 \retval skin minor id.
 */
int CaIconDescriptionPrivate::skinMinorId() const
{
    return mSkinMinorId;
}

/*!
 Sets skin minor id.
 \param skin minor id.
 */
void CaIconDescriptionPrivate::setSkinMinorId(int id)
{
    mSkinMinorId = id;
}

/*!
 Sets icon id.
 \param id icon id.
 */
void CaIconDescriptionPrivate::setId(int id)
{
    mId = id;
}

