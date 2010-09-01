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


#ifndef C_PROFILECHANGENOTIFYHANDLERSTUB_H
#define C_PROFILECHANGENOTIFYHANDLERSTUB_H


#include <e32base.h>
#include <mprofileengine.h>
#include <mprofile.h>


class CProfile : public CBase, public MProfile
    {
        ~CProfile();
public:
        virtual void Release();
        virtual const TArray<TContactItemId> AlertForL();
        virtual TBool IsSilent() const;
        virtual const MProfileName& ProfileName() const;
        virtual const MProfileTones& ProfileTones() const;
        virtual const MProfileExtraTones& ProfileExtraTones() const;
        virtual const MProfileExtraSettings& ProfileExtraSettings() const;
    };
/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CProfileEngine : public CBase, public MProfileEngine
    {
public:

    static CProfileEngine* NewL();

    static CProfileEngine* NewLC();

    virtual ~CProfileEngine();

    void Release();
    MProfile* ActiveProfileLC();
    MProfile* ActiveProfileL();
    TInt ActiveProfileId();
    MProfilesNamesArray* ProfilesNamesArrayLC();
    void SetActiveProfileL( TInt aId );
    void SetTempRingingVolumeL( TProfileRingingVolume aVolume );
    TProfileRingingVolume TempRingingVolumeL() const;
    void SetTempMediaVolumeL( TProfileRingingVolume aVolume );
    TProfileRingingVolume TempMediaVolumeL() const;
    TBool IsFeatureSupported( TProfileFeatureId aFeatureId ) const;
    TBool IsActiveProfileTimedL();

private:

    CProfileEngine();

    void ConstructL();

private: // data
    };


#endif // C_PROFILECHANGENOTIFYHANDLERSTUB_H
