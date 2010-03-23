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
* Description:  HsService base 
*
*/


#ifndef SERVICEBASE_H
#define SERVICEBASE_H

struct HsServiceInterfaceId;

/** @ingroup group_hsservicemodel group_service_api_candidates
 *  The base class for service interfaces
 *
 *  @lib 
 *  @since S60 ?S60_version
 */
class IHsServiceBase
    {
    
public:
    /**
     * Default virtual destructor.
     */
    virtual ~IHsServiceBase() {}

    /**
     * Interface getter with casting.
     *
     * @since S60 ?S60_version
     * @param aBase HsService base object.
     * @return The queried interface, or NULL if the interface is not
     *         supported or available.
     */
    template <class T>
    static T* MakeInterface(IHsServiceBase* aBase)
        {
        return (aBase? static_cast<T*>(aBase->resolveService(T::type())): 0);    
        }
        
    
    /**
     * Interface getter.
     * Derived classes should always call the base class method
     * from the overridden MakeInterface.
     *
     * @since S60 ?S60_version
     * @param aType The type id of the queried interface.
     * @return The queried interface, or NULL if the interface is not
     *         supported or available.
     */
    virtual IHsServiceBase* resolveService( const HsServiceInterfaceId &aType ) = 0;
    
    
    };

#endif
