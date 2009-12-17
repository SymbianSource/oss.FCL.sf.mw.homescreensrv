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
* Description:
*
*/

class CTestUtils: public CBase
{
public:

    /**/
    virtual ~CTestUtils();

    /**/
    static CTestUtils* NewL();

    /**/
    static CTestUtils* NewLC();
   
    TInt Copy( const TDesC& aFileName, TInt aSuffix = 0 );
    TInt CopyImport( const TDesC& aFileName, TInt aSuffix = 0 );
    TInt Copy( const TDesC& aSource, const TDesC& aDest );
    TInt Delete( /*const TDesC& aFilePath*/ );
    void InstallSisL( const TDesC& aName );
    void UninstallSisL( const TDesC& aName );

private:


    CTestUtils();

    void ConstructL();


private:
    RFs iFs;
    CFileMan* iFileManager;
};
