@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off

if exist homescreensrv_ut_hs.sisx del homescreensrv_ut_hs.sisx

makesis homescreensrv_ut_hs.pkg
signsis homescreensrv_ut_hs.sis homescreensrv_ut_hs.sisx rd.cer rd-key.pem

if exist homescreensrv_ut_hs.sisx (
echo homescreensrv_ut_hs.sisx creation SUCCEEDED
del homescreensrv_ut_hs.sis
)

if not exist homescreensrv_ut_hs.sisx (
echo homescreensrv_ut_hs.sisx creation FAILED
)