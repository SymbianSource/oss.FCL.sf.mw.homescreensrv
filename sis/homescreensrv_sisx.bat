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

if exist homescreensrv.sisx del homescreensrv.sisx

makesis homescreensrv.pkg
signsis homescreensrv.sis homescreensrv.sisx rd.cer rd-key.pem

if exist homescreensrv.sisx (
echo homescreensrv.sisx creation SUCCEEDED
del homescreensrv.sis
)

if not exist homescreensrv.sisx (
echo homescreensrv.sisx creation FAILED
)