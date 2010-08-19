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
* Description:  
*
*/

#ifndef __INTERFACE_EU_H__
#define __INTERFACE_EU_H__

// External includes
#include <e32base.h>
#include <liwvariant.h>

/**
 * Observer class
 */
class Interface_EU : public CBase, 
    public MLiwInterface
    {
public:
    static Interface_EU* NewL();

    ~Interface_EU();

    void ExecuteCmdL(const TDesC8& aCmdName,
                     const CLiwGenericParamList& aInParamList,
                     CLiwGenericParamList& aOutParamList,
                     TUint aCmdOptions = 0,
                     MLiwNotifyCallback* aCallback = 0);
    
    void Close();
    
protected:
    void ConstructL();

private:
    Interface_EU();
    };

#endif // __INTERFACE_EU_H__

// End of File.
