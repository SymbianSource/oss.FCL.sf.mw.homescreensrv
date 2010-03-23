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
 * Description: declaration of functions providing string constants
 *
 */

#ifndef HOMESCREEN_DOMAIN_PS_KEYS_H
#define HOMESCREEN_DOMAIN_PS_KEYS_H

/**
 * P&S UID for all Home Screen state information keys provided by this API.
 */
const QUuid KPSUidHsState("0x20022F75");

/**
 * P&S Key path for all Home Screen state information keys provided by this API.
 */
const QString HsStatePSKeyPath = "/homescreeninformation";

/**
 * P&S Key sub path to represent the state of the Home Screen. This property is updated when
 * Home screen state is changed. Valid values are defined by the enum EPSHsState
 * and range from 0 to 1.
 *
 * @see EPSHsState
 */
const QString HsStatePSKeySubPath = "resources/homescreen/state";

/**
 * Home screen state.
 */
enum EPSHsState
     {
     /**
     Invalid state. P&S key not initialized.
     */
     EHomeScreenInvalidState = 0,
     /**
     Home Screen inactive
     */
     EHomeScreenInactive = 1,
     /**
     Home Screen in idle state
     */
     EHomeScreenIdleState   = 2
     };

#endif //HOMESCREEN_DOMAIN_PS_KEYS_H