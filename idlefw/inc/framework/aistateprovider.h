/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State Provider
*
*/


#ifndef _AISTATEPROVIDER_H
#define _AISTATEPROVIDER_H

// System includes
#include <e32base.h>
#include <coemain.h>
#include <hwrmlight.h>
#include <AknsSrvClient.h>

// User includes
#include <aifwstatehandler.h>
#include "aiecomobserver.h"

// Forward declarations
class MAiPSPropertyObserver;
class MAiStateObserver;
class THsPublisherInfo;

// Class declaration

/**
 * State Provider
 * 
 * @ingroup group_aifw
 * @lib aifw.lib
 * @since S60 5.2
 */
NONSHARABLE_CLASS( CAiStateProvider ) : public CBase,
    public MCoeMessageMonitorObserver,
    public MHWRMLightObserver,
    public MAknsSkinChangeObserver,
    public MAiEcomObserver,
    public MAiFwStateHandler
    {
public:    
    // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    static CAiStateProvider* NewL( MAiStateObserver& aObserver ); 
        
    /**
     * Two-phased constructor. Leaving on stack
     */
    static CAiStateProvider* NewLC( MAiStateObserver& aObserver );         

    /**
     * Destructor
     */
    ~CAiStateProvider();

private:
    // private constructors

    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * C++ default constructor
     */
    CAiStateProvider( MAiStateObserver& aObserver );         
     
public:
    // new methods
    
    /**
     * Start state providing
     * 
     * @since S60 5.2
     * @param aCoeEnv Control environment
     */
    void StartL( CCoeEnv& aCoeEnv );
    
    /**
     * Stop state providing
     * 
     * @since S60 5.2
     */
    void Stop();
    
private:
    // from MCoeMessageMonitorObserver
    
    /**
     * @see MCoeMessageMonitorObserver
     */
    void MonitorWsMessage( const TWsEvent& aEvent );
    
private:
    // from MHWRMLightObserver
    
    /**
     * @see MHWRMLightObserver
     */
    void LightStatusChanged( TInt aTarget, 
        CHWRMLight::TLightStatus aStatus );
    
private:
    // from MAknsSkinChangeObserver
    
    /**
     * @see MAknsSkinChangeObserver
     */
    void SkinContentChanged();

    /**
     * @see MAknsSkinChangeObserver
     */    
    void SkinConfigurationChanged(
        const TAknsSkinStatusConfigurationChangeReason aReason );
    
    /**
     * @see MAknsSkinChangeObserver
     */    
    void SkinPackageChanged(
        const TAknsSkinStatusPackageChangeReason aReason );
    
private:
    // from MAiEcomObserver
    
    /**
     * @see MAiEcomObserver
     */
    void NotifyEcomRegistryChanged();
    
private:
    // from MAiFwStateHandler

    /**
     * @see MAiFwStateHandler
     */
    TInt LoadPlugin( const THsPublisherInfo& aPublisherInfo, 
        TAiFwLoadReason aReason );
    
    /**
     * @see MAiFwStateHandler
     */
    void DestroyPlugin( const THsPublisherInfo& aPublisherInfo,
        TAiFwDestroyReason aReason );
    
    /**
     * @see MAiFwStateHandler
     */
    void ChangePluginState( TAiFwState aState );

    /**
     * @see MAiFwStateHandler
     */    
    TBool OnlineStateInUse() const;

private:
    // new functions
    
    static TInt BackupRestoreEvent( TAny* aAny );
    
private:
    // data

    /** ECom observer, owned */
    CAiEcomObserver* iEcomObserver;        
    /** State observer, Not owned */
    MAiStateObserver& iObserver;        
    /** Control environment, Not owned */
    CCoeEnv* iCoeEnv;
    /** Light status observer, Owned */
    CHWRMLight* iLightObserver;
    /** Skin server session, Owned */
    RAknsSrvSession iSkinSrv;
    /** Backup Restore observer, Owned */
    MAiPSPropertyObserver* iBackupRestoreObserver;
    /** Flag to indicate whether state providing is started */
    TBool iStarted;    
    
private:
    // friend classes
    
#ifdef _AIFW_UNIT_TEST
    friend class UT_AiStateProvider;
#endif    
    };

#endif // _AISTATEPROVIDER_H

// End of file

