#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

'''
Finite State Machine generator for Symbian OS projects.

Just define fsmName, actionsIfaceName and stt and the script generates the
needed files for you.

The files are:
- State Machine header file (including state classes, overwrites the existing file)
- State Machine source file (including state classes, overwrites the existing file)
- Actions interface header (abstract M-class, overwrites the existing file)
- Header file for tracing (will be generated only if missing)


Version 1: 22-Jun-2006, Jaakko Vuori, Initial version
Version 2: 26-Jun-2006, Jaakko Vuori, Added argument passing
Version 3: 27-Jun-2006, Jaakko Vuori, Changed state classes nested in FSM class

'''
from sets import Set
import os, re, string

#------------------------------------------------------------------------
#State machine name
fsmName = "Popup"

#Actions interface name
actionsIfaceName = "PopupFSMActions"

#State transition table
stt=[   #source state       #event                            #dest state         #actions
        ("NotSetOffFocus",  [
                            ("HandleUpdate()",                "SetOffFocus",      ["IssueCleanCaption()"]),
                            ("HandleGotFocus()",              "NotSetOnFocus",    []),
                            ("HandleForeground()",            "NotSetOffFocus",   ["IssuePublishCaption()"]),
                            ]),    

        ("SetOffFocus",     [
                            ("HandleGotFocus()",              "GettingFocus",     ["StartShortTimer()"]),
                            ("HandleReset()",                 "NotSetOffFocus",   ["IssuePublishCaption()"]),
                            ]),    

        ("GettingFocus",    [
                            ("HandleRequestCompleted()",      "Visible",          ["IssuePublishPopup()", "StartLongTimer()"]),
                            ("HandleReset()",                 "NotSetOnFocus",    ["IssuePublishCaption()", "CancelRequest()"]),
                            ("HandleLostFocus()",             "SetOffFocus",      ["CancelRequest()"]),
                            ]),    
                            
        ("LosingFocus",     [
                            ("HandleRequestCompleted()",      "SetOffFocus",      ["IssueCleanPopup()"]),
                            ("HandleReset()",                 "NotSetOffFocus",   ["IssuePublishCaption()", "CancelRequest()"]),
                            ("HandleGotFocus()",              "Visible",          ["CancelRequest()", "StartLongTimer()"]),
                            ]),
                            
        ("NotSetOnFocus",   [
                            ("HandleUpdate()",                "Visible",          ["IssueCleanCaption()", "IssuePublishPopup()", "StartLongTimer()"]),
                            ("HandleLostFocus()",             "NotSetOffFocus",   []),
                            ("HandleBackground()",            "BackgroundNotSetOnFocus", []),
                            ("HandleForeground()",            "NotSetOnFocus",    ["IssuePublishCaption()"]),
                            ]),    
                                                        
        ("Visible",         [
                            ("HandleUpdate()",                "Visible",              ["IssueCleanCaption()", "IssuePublishPopup()", "StartLongTimer()"]),
                            ("HandleReset()",                 "NotSetOnFocus",        ["IssuePublishCaption()", "IssueCleanPopup()", "CancelRequest()"]),
                            ("HandleLostFocus()",             "LosingFocus",          ["CancelRequest()", "CompleteSelf()"]),
                            ("HandleRequestCompleted()",      "NotVisible",           ["IssueCleanPopup()"]),
                            ("HandleBackground()",            "BackgroundSetOnFocus", ["CancelRequest()", "IssueCleanPopup()"]),
                            ]),
                            
        ("NotVisible",      [
                            ("HandleUpdate()",                "Visible",          ["IssueCleanCaption()", "IssuePublishPopup()", "StartLongTimer()"]),
                            ("HandleReset()",                 "NotSetOnFocus",    ["IssuePublishCaption()"]),
                            ("HandleLostFocus()",             "SetOffFocus",      []),
                            ]),

        ("BackgroundNotSetOnFocus",      [
                            ("HandleForeground()",            "NotSetOnFocus",        ["IssuePublishCaption()"]),
                            ("HandleUpdate()",                "BackgroundSetOnFocus", ["IssueCleanCaption()"]),
                            ]),

        ("BackgroundSetOnFocus",      [
                            ("HandleReset()",                 "BackgroundNotSetOnFocus",        ["IssuePublishCaption()"]),
                            ("HandleForeground()",            "Visible",                        ["IssuePublishPopup()", "StartLongTimer()"]),
                            ]),

    ]
#------------------------------------------------------------------------    

def write( s ):
    f.write( s + '\n' )

def writeHeader():
    global f
    fname = '%sFSM.h' % (fsmName)
    print "Generating %s..." % (fname)
    f = open( fname, 'w' )

    write( "#ifndef %sFSM_H" % ( fsmName.upper() ) )
    write( "#define %sFSM_H" % ( fsmName.upper() ) )
    write( "" )
    write( "//  EXTERNAL INCLUDES" )
    write( "#include <e32base.h>" )
    write( "" )
    write( "//  FORWARD DECLARATIONS" )
    write( "class T%sFSM;" % (fsmName) )
    write( "class M%s;" % (actionsIfaceName) )
    write( "" )
    write( "//  CLASS DEFINITIONS" )

    write( "/**" )
    write( " * %s state machine" % (fsmName) )
    write( " */" )
    write( "class T%sFSM" % (fsmName) )
    write( "    {" )

    write( "    /**" )
    write( "     * Base class for states" )
    write( "     */" )
    write( "    class T%sStateBase" % (fsmName) )
    write( "        {" )
    write( "        public:     // New methods" )
    
    events=[]
    for s in stt:
        for e in s[1]:
            events.append( e[0] )    
    for event in Set(events):
        fname, args = re.match('(.*?)[(](.*?)[)]',event).groups()
        if len(args)>0:
            write( "            virtual void %s( T%sFSM* a%sFSM, M%s& a%s,%s);" % (fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName,args) )
        else:
            write( "            virtual void %s( T%sFSM* a%sFSM, M%s& a%s );" % (fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName) )
        
        
    write( "        };" )
    write( "" )
    
    for s in stt:
        write( "    /**" )
        write( "     * State class T%s" % (s[0]) )
        write( "     */" )    
        write( "    class T%s : public T%sStateBase" % (s[0],fsmName) )
        write( "        {" )
        write( "        protected:  // Methods derived from T%sState" % (fsmName))
        for event in s[1]:        
            fname, args = re.match('(.*?)[(](.*?)[)]',event[0]).groups()
            if len(args)>0:
                write( "            void %s( T%sFSM* a%sFSM, M%s& a%s,%s);" % (fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName,args) )
            else:
                write( "            void %s( T%sFSM* a%sFSM, M%s& a%s );" % (fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName) )
            
        write( "        };" )
        write( "" )

    write( "" )
    write( "    public:    // Constructors" )
    write( "        T%sFSM( M%s& a%s );" % (fsmName,actionsIfaceName,actionsIfaceName))
    write( "" )
    write( "    public:     // New methods" )
    for event in Set(events):            
        write( "        void %s;" % (event))
    write( "" )
    write( "    private:     // New methods" )
    write( "        void SetState( T%sStateBase* aNewState );" % (fsmName) )
    write( "" )
    write( "    private:    // Data" )
    write( "        //Ref:" )
    write( "        T%sStateBase* iCurrentState;" % (fsmName))
    write( "        M%s& i%s;" % (actionsIfaceName,actionsIfaceName))
    write( "" )
    write( "        //Own:" )
    for s in stt:
        write( "        T%s i%s;" % (s[0],s[0]))

    write( "    private:    // Friend class definitions" )
    for s in stt:
        write( "        friend class T%s;" % (s[0]))
        
    write( "    };" )
    write( "" )
    write( "#endif      //  %sFSM_H" % (fsmName.upper()) )
    write( "" )
    write( "// End of file" )
    f.close()


def writeSource():
    global f
    fname = '%sFSM.cpp' % (fsmName)
    print "Generating %s..." % (fname)    
    f = open( fname, 'w' )

    write( '#include "%sFSM.h"' % (fsmName) )
    write( '#include "M%s.h"' % (actionsIfaceName) )
    write( '#include "%sTrace.h"' % (fsmName) )
    write( '' )
    write( '#pragma warning( disable:4100 )' )    
    write( '#pragma warning( disable:4068 )' )        
    write( '#pragma warn_unusedarg off' )    
    
    write( '' )
    write( 'T%sFSM::T%sFSM( M%s& a%s ) : ' % (fsmName,fsmName,actionsIfaceName,actionsIfaceName) )
    write( '    iCurrentState( &i%s ),' % (stt[0][0]) )
    write( '    i%s( a%s )' % (actionsIfaceName,actionsIfaceName) )
    write( '    {' )
    write( '    }' )
    write( '' )
    
    events=[]
    for s in stt:
        for e in s[1]:
            events.append( e[0] )    
            
    for event in Set(events):                
        fname, args = re.match('(.*?)[(](.*?)[)]',event).groups()
        if len(args)>0:
            write( 'void T%sFSM::T%sStateBase::%s( T%sFSM* /*a%sFSM*/, M%s& /*a%s*/,%s )' % (fsmName,fsmName,fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName,args))
        else:
            write( 'void T%sFSM::T%sStateBase::%s( T%sFSM* /*a%sFSM*/, M%s& /*a%s*/ )' % (fsmName,fsmName,fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName))
        write( '    {' )        
        write( '    TRACE( _L("T%sStateBase::%s") );' % (fsmName,event))
        write( '    }' )
        write( '' )

    for event in Set(events):                
        fname, args = re.match('(.*?)[(](.*?)[)]',event).groups()
        write( 'void T%sFSM::%s(%s)' % (fsmName,fname,args) )
        write( '    {' )
        arglist=args.split(',')
        argnames = string.join([arg.split()[-1:][0] for arg in arglist if arg!=""], ', ')
        if len(argnames)>0:
            write( '    iCurrentState->%s( this, i%s, %s );' % (fname,actionsIfaceName,argnames) )
        else:
            write( '    iCurrentState->%s( this, i%s );' % (fname,actionsIfaceName) )
        write( '    }' )
        write( '' )

    write( 'void T%sFSM::SetState( T%sStateBase* aNewState )' % (fsmName,fsmName) )
    write( '    {' )
    write( '    iCurrentState = aNewState;' )
    write( '    }' )
    write( '' )

    for s in stt:
        stateName, events = s
        for event in events:
            eventName = event[0]
            destState = event[1]
            actions = event[2]
            fname, args = re.match('(.*?)[(](.*?)[)]',eventName).groups()
            if len(actions)>0:                
                if len(args)>0:
                    write( 'void T%sFSM::T%s::%s( T%sFSM* a%sFSM, M%s& a%s,%s )' % (fsmName,stateName,fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName,args) )
                else:
                    write( 'void T%sFSM::T%s::%s( T%sFSM* a%sFSM, M%s& a%s )' % (fsmName,stateName,fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName) )
            else:
                if len(args)>0:
                    write( 'void T%sFSM::T%s::%s( T%sFSM* a%sFSM, M%s& /*a%s*/,%s )' % (fsmName,stateName,fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName,args) )
                else:
                    write( 'void T%sFSM::T%s::%s( T%sFSM* a%sFSM, M%s& /*a%s*/ )' % (fsmName,stateName,fname,fsmName,fsmName,actionsIfaceName,actionsIfaceName) )
            
            write( '    {' )
            write( '    TRACE( _L("T%s::%s") );' % (stateName, eventName) )
            write( '    a%sFSM->SetState( &a%sFSM->i%s );' % (fsmName,fsmName,destState) )
            for action in actions:
                fname, args = re.match('(.*?)[(](.*?)[)]',action).groups()
                if len(args)>0:
                    arglist=args.split(',')                
                    argnames = string.join([arg.split()[-1:][0] for arg in arglist if arg!=""], ', ')
                    write( '    a%s.%s( %s );' % (actionsIfaceName,fname,argnames) )
                else:
                    write( '    a%s.%s();' % (actionsIfaceName,fname) )
            write( '    }' )
            write( '' )
        
    write( '// End of file' )


def writeTraceHeader():
    if not os.path.isfile( '%sTrace.h' % (fsmName) ):
        global f
        fname = '%sTrace.h' % (fsmName)
        print "Generating %s..." % (fname)            
        f = open( fname, 'w' )
        write( '#ifndef %sTRACE_H' % (fsmName.upper()) )
        write( '#define %sTRACE_H' % (fsmName.upper()) )
        write( '' )
        write( '//  MACROS' )
        write( '#define TRACE' )
        write( '' )
        write( '#endif      //  %sTRACE_H' % (fsmName.upper()) )
        write( '' )
        write( '// End of file' )
        f.close()


def writeActionsIface():
    global f
    fname = 'M%s.h' % (actionsIfaceName)
    print "Generating %s..." % (fname)            
    f = open( fname, 'w' )

    write( '#ifndef M%s_H' % (actionsIfaceName.upper()) )
    write( '#define M%s_H' % (actionsIfaceName.upper()) )
    write( '' )
    write( '//  EXTERNAL INCLUDES' )
    write( '#include <e32def.h>' )
    write( '' )
    write( '//  CLASS DEFINITION' )
    write( '/**' )
    write( ' * M%s actions.' % (actionsIfaceName) )
    write( ' * Note: This file has been generated automatically. Do not edit!' )
    write( ' */' )
    write( 'class M%s' % (actionsIfaceName) )
    write( '    {' )
    write( '    public:     // Abstract methods' )

    actions=[]
    for s in stt:
        for e in s[1]:
            actions.extend( e[2] )    
    for action in Set(actions):
        write( '        virtual void %s = 0;' % (action))
    write( '    };' )
    write( '' )
    write( '#endif      //  M%s_H' % (actionsIfaceName.upper()) )
    write( '' )
    write( '// End of file' )
    f.close()
    
if __name__ == "__main__":
    writeHeader()
    writeSource()
    writeTraceHeader()
    writeActionsIface()
