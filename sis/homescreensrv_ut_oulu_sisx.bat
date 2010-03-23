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

if exist homescreensrv_ut_oulu.sisx del homescreensrv_ut_oulu.sisx

makesis homescreensrv_ut_oulu.pkg
signsis homescreensrv_ut_oulu.sis homescreensrv_ut_oulu.sisx rd.cer rd-key.pem

if exist homescreensrv_ut_oulu.sisx (
echo homescreensrv_ut_oulu.sisx creation SUCCEEDED
del homescreensrv_ut_oulu.sis
)

if not exist homescreensrv_ut_oulu.sisx (
echo homescreensrv_ut_oulu.sisx creation FAILED
)