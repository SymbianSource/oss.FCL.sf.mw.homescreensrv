@rem
@rem Copyright (c) 20010 Nokia Corporation and/or its subsidiary(-ies).
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

makesis test_themes.pkg
signsis test_themes.sis test_themes.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
del test_themes.sis

