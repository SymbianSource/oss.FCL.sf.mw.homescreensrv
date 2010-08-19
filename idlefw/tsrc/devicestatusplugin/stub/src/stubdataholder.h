/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef C_STUBDATAHOLDER_H
#define C_STUBDATAHOLDER_H


#include <e32base.h>
#include <mprofilename.h>
#include <nwhandlingengine.h>
#include <rsssettings.h>
#include <msssettingsobserver.h>


class MProfileChangeObserver;

class CProfileName : public CBase, public MProfileName
    {
public:

    ~CProfileName();
    TInt Id() const;
    const TDesC& Name() const;
    const TDesC& NaviName() const;
    void SetL( const TDesC& aName, TInt aId );
    const TDesC& ShortName() const;
	const TDesC& TitleName() const;
	
private:

    HBufC* iName;
    HBufC* iNaviName;
    HBufC* iTitleName;
    TInt iId;
    };



class TSSSettingsValue
    {
public:
    TSSSettingsSetting iSetting;
    TInt iValue;
    };


class TProfileObserver
    {
public:
    CBase* iOwner;
    MProfileChangeObserver* iObserver; 
    };

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CStubDataHolder : public CBase
    {
public:

    IMPORT_C static CStubDataHolder* InstanceL();
    IMPORT_C static CStubDataHolder* Instance();
    IMPORT_C static void Release();
    virtual ~CStubDataHolder();



    IMPORT_C void AddProfileChangeObserverL( MProfileChangeObserver& aObserver, CBase* aOwner );
    IMPORT_C void RemoveProfileChangeObserver( CBase* aOwner );
    IMPORT_C RArray<TProfileObserver>& ProfileChangeObservers();

    IMPORT_C void SetCurrentProfileL( const TDesC& aName, TInt aId );
    IMPORT_C MProfileName* CurrentProfile();

    IMPORT_C void SetNWMessageObserver( MNWMessageObserver& aObserver, TNWInfo& aInfo );
    IMPORT_C MNWMessageObserver* NWMessageObserver();
    IMPORT_C TNWInfo* NWInfo();


    void AddSSSettingsObserverL( MSSSettingsObserver& aObserver );
    void RemoveSSSettingsObserver( MSSSettingsObserver& aObserver );
    TInt GetSSSettingsValueL( TSSSettingsSetting aSetting );
    IMPORT_C RArray<MSSSettingsObserver*>& SSSettingsObservers();
    IMPORT_C void SetSSSettingsValueL( TSSSettingsSetting aSetting, TInt aValue );
    IMPORT_C void RemoveSSSetting( TSSSettingsSetting aSetting );
    

protected:

private:

    CStubDataHolder();

    void ConstructL();

private: // data

    RArray<TProfileObserver> iProfileChangeObservers;
    CProfileName* iProfile;
    MNWMessageObserver* iNWObserver;
    TNWInfo* iNWInfo;
    RArray<TSSSettingsValue> iSSSettingValues;
    RArray<MSSSettingsObserver*> iSSSettingsObservers;
    };


#endif // C_STUBDATAHOLDER_H
