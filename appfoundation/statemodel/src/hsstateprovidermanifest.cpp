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
* Description:  State provider manifest.
*
*/


#include <QDomDocument>
#include <QFile>

#include "hsstateprovidermanifest.h"
#include "hsstateprovidermanifest_p.h"
#include "hstest_global.h"

HsStateProviderManifestPrivate::HsStateProviderManifestPrivate(HsStateProviderManifest* aPublic)
    : QObject(aPublic),
      mQ(aPublic),
      mLoadOnQuery(false)
{
}

HsStateProviderManifestPrivate::~HsStateProviderManifestPrivate()
{

}

bool HsStateProviderManifestPrivate::loadFromXml(const QString& aFileName)
{
    mStateTokens.clear();

    QFile file(aFileName);
    if(!file.exists())
    {
        return false;
    }

    QDomDocument document;
    if(!document.setContent(&file))
    {
        return false;
    }

    QDomElement element = document.documentElement();
    if(element.tagName() != "stateprovider")
    {
        return false;
    }

    mLoadOnQuery = false;
    QDomAttr attribute = element.attributeNode("loadonquery");
	if(attribute.value().toLower() == "true")
	{
        mLoadOnQuery = true;
	}
    
    QList<HsStateToken> tokens;
    HsStateToken token;

    QDomNodeList states = element.elementsByTagName("state");

    for(int i = 0; i < states.count(); ++i)
    {
        element = states.at(i).toElement();
        token.mLibrary = parseAttribute(element, "library");
        token.mUri = parseAttribute(element, "uri");
        tokens << token;
    }

    mStateTokens = tokens;
    return true;
}

QList<HsStateToken> HsStateProviderManifestPrivate::states() const
{
    return mStateTokens;
}

QString HsStateProviderManifestPrivate::parseAttribute(QDomElement& aElement, const QString& aAttributeName, bool aIsRequired) const
{
    QDomAttr attribute = aElement.attributeNode(aAttributeName);
    if(attribute.isNull() || attribute.value().isEmpty())
    {
        if(aIsRequired)
        {
            HSDEBUG("Required attribute missing")            
        }
        else
        {
            HSDEBUG("Attribute missing")
        }

        return QString();
    }

    return attribute.value();
}


/*!
    \class HsStateProviderManifest
    \ingroup group_hsstatemodel
    \brief Loads home screen state tokens from an XML manifest file.
    Used by the HsStateFactory for loading state tokens from an XML
    manifest file.
*/

/*!
    Constructor. Loads states as state tokens from the given
    manifest file.
    \a aParent Parent object.
*/
HsStateProviderManifest::HsStateProviderManifest(QObject* aParent)
    : QObject(aParent)
{
    mD = new HsStateProviderManifestPrivate(this);
}

/*!
    Destructor.
*/
HsStateProviderManifest::~HsStateProviderManifest()
{

}

/*!
    Returns loaded states as state tokens.
*/
QList<HsStateToken> HsStateProviderManifest::states() const
{
    return mD->states();
}

/*!
    Returns true if the state tokens must be asked from plugin
    instead of manifest file.
*/
bool HsStateProviderManifest::loadOnQuery()const
{
	return mD->mLoadOnQuery;
}

/*!
    Loads states as state tokens from a manifest file.
    \a aFileName Manifest file name.

    Returns true if load was succesfull, false otherwise
*/
bool HsStateProviderManifest::loadFromXml(const QString& aFileName)
{
    return mD->loadFromXml(aFileName);
}

