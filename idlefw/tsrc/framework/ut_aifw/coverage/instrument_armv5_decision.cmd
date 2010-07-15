@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
@rem 
@rem Description:
@rem 
@rem 



@ECHO OFF
setlocal

ECHO ********** Clean the environment **********

call sbs -c armv5_urel reallyclean -k
call del profile.txt
call del mon.sym
call del mon.dat

ECHO ********** Build module tests and Instrument test subjects **********

call ctcwrap -i d -2comp sbs -c armv5_urel

ECHO Generate image now. When you're done, copy ctcdata.txt into this directory
ECHO and call generate.cmd