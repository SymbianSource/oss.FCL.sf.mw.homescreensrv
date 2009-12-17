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
* Description:  Class stores basic information about application
 *
*/


#ifndef BSAPPLICATIONINFO_H
#define BSAPPLICATIONINFO_H

/**
 *  Simple helper class used for keeping basic information in back stepping stack
 * 
 *  @since S60 3.2
 */
class CBSApplicationInfo : public CBase
    {
public:

    static CBSApplicationInfo* NewL();
    static CBSApplicationInfo* NewLC();

    /**
     * Destructor.
     */
    ~CBSApplicationInfo();

    /**
     * Set application UID
     *
     * @since S60 3.2
     * @param aApp application UID
     */
    void SetAppUid( const TUid& aApp );

    /**
     * Get application UID
     *
     * @since S60 3.2
     * @return application UID
     */
    TUid AppUid();

    /**
     * Set previous application UID
     * It is application that started application in question
     *
     * @since S60 3.2
     * @param aApp previous application UID
     */
    void SetPrevAppUid( const TUid& aApp );

    /**
     * Get previous application UID
     *
     * @since S60 3.2
     * @return previous application UID
     */
    TUid PrevAppUid();

    /**
     * Set item state of application.
     * It is a state that was activated when application gained focus
     * Item state is encoded in simple descriptor
     *
     * @since S60 3.2
     * @param aState application item state
     */
    void SetItemStateL( const TDesC8& aState );

    /**
     * Get item state of application.
     * It is a state that was activated when application gained focus
     * Item state is encoded in simple descriptor
     *
     * @since S60 3.2
     * @return application item state
     */
    const TDesC8& ItemState();

private:

    CBSApplicationInfo();

    void ConstructL();

private:
    // data
    /**
     * Applicatin UID
     */
    TUid iAppUid;

    /**
     * Previous applicatin UID
     */
    TUid iPrevAppUid;

    /**
     * Item State of application
     * Own. 
     */
    HBufC8* iItemState;

    };

#endif // BSAPPLICATIONINFO_H
