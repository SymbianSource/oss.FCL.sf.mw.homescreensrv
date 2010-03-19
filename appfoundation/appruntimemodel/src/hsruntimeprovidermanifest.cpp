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
* Description:  HsRuntime provider manifest.
*
*/


#include "hsruntimeprovidermanifest.h"
#include "hsruntimeprovidermanifest_p.h"
#include "hstest_global.h"

#include <QDomDocument>
#include <QFile>

HsRuntimeProviderManifestPrivate::HsRuntimeProviderManifestPrivate(HsRuntimeProviderManifest* aPublic)
    : QObject(aPublic),
      mQ(aPublic),
      mLoadOnQuery(false)
{

}

HsRuntimeProviderManifestPrivate::~HsRuntimeProviderManifestPrivate()
{

}

bool HsRuntimeProviderManifestPrivate::loadFromXml(const QString& aFileName)
{
    mRuntimeTokens.clear();

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
    if(element.tagName() != "runtimeprovider")
    {
        return false;
    }

    mLoadOnQuery = false;
    QDomAttr attribute = element.attributeNode("loadonquery");
	if(attribute.value().toLower() == "true")
	{
        mLoadOnQuery = true;
	}

    QList<HsRuntimeToken> tokens;
    HsRuntimeToken token;

    QDomNodeList runtimes = element.elementsByTagName("runtime");

    for(int i = 0; i < runtimes.count(); ++i)
    {
        element = runtimes.at(i).toElement();
        token.mLibrary = parseAttribute(element, "library");
        token.mUri = parseAttribute(element, "uri");
        tokens << token;
    }

    mRuntimeTokens = tokens;
    return true;
}

QList<HsRuntimeToken> HsRuntimeProviderManifestPrivate::runtimes() const
{
    return mRuntimeTokens;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
QString HsRuntimeProviderManifestPrivate::parseAttribute(QDomElement& aElement, 
                                                       const QString& aAttributeName, 
                                                       bool aIsRequired) const
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
    \class HsRuntimeProviderManifest
    \ingroup group_hsruntimemodel
    \brief Loads home screen HsRuntime tokens from an XML manifest file.
    Used by the HsRuntimeFactory for loading HsRuntime tokens from an XML
    manifest file.
        
*/

/*!
    Constructor. Loads runtimes as HsRuntime tokens from the given
    manifest file.
    \a aParent Parent object.
*/
HsRuntimeProviderManifest::HsRuntimeProviderManifest(QObject* aParent)
    : QObject(aParent)
{
    mD = new HsRuntimeProviderManifestPrivate(this);
}


/*!
    Destructor.    
*/
HsRuntimeProviderManifest::~HsRuntimeProviderManifest()
{

}

/*!
    return loaded runtimes as HsRuntime tokens.    
*/
QList<HsRuntimeToken> HsRuntimeProviderManifest::runtimes() const
{
    return mD->runtimes();
}

/*!
    Tells if the HsRuntime tokens must be asked from plugin
    instead of manifest file. 

    return True if loading required, false otherwise.
*/
bool HsRuntimeProviderManifest::loadOnQuery() const
{
	return mD->mLoadOnQuery;
}


/*!
    Loads runtimes as HsRuntime tokens from a manifest file.    
    \a aFileName Manifest file name.

    return true if load was succesfull, false otherwise
*/
bool HsRuntimeProviderManifest::loadFromXml(const QString& aFileName) const
{
	return mD->loadFromXml(aFileName);
}
