/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Updates applications and icons in Operator Tile.
*
*/


#ifndef __HSWIDGETITEM_H__
#define __HSWIDGETITEM_H__

#include <string>

namespace Hs {

class HsWidgetItem
{
public:
    
    /**
     */
    HsWidgetItem( std::string& aItemName, std::string& aValue );
    
    /**
     */
    HsWidgetItem( std::string& aItemName, int aValue );
    
    /**
     */
    virtual ~HsWidgetItem();
    
    /**
     */
    void setValue( int aValue );
    
    /**
     */
    void setValue( std::string& aValue );

    /**
     */
    const std::string& getItemName();
    
    /**
     */
    const std::string& getItemValue();
    
    /**
     */
    int getItemValueInt();

    /**
     */
    bool isStringValue();

    
private:
    std::string mItemName;
    std::string mItemString;
    int 		mItemInt;
    bool 		mIsString;
    };
}

#endif /*__HSWIDGETITEM_H__*/
