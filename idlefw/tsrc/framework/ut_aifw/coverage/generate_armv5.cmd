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

ECHO ********** Build coverage report **********

call "C:\Apps\ctc\Sym_armv5\ctcrecvr.exe" -i ctcdata.txt
call ctcpost -p profile.txt
call ctc2html -i profile.txt
