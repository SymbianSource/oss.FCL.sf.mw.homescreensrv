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
#!/usr/bin/perl
use XML::Twig;
my $twig= new XML::Twig();

$logpath = "\\epoc32\\winscw\\c\\shared\\EUnit\\Logs\\EUnit_log.xml";
$twig->parsefile( $logpath);

my $root= $twig->root;
my $elt = $twig->root;
my $failCount = 0;
my $okCount = 0;

while ($elt = $elt->next_elt('result')){
    if( $elt->att('status') eq "FAIL")
    	{
    	$failCount++;	
  		}
    elsif( $elt->att('status') eq "OK")
    	{
    	$okCount++;	
  		}
}

my $allCount = $failCount + $okCount;
print "\nUnit testing results:\n\n";
print $allCount;
print " unit tests run.\n";
print $okCount;
print " unit tests passed.\n";
print $failCount;
print " unit tests failed.\n";
print "\n";

if( $failCount > 0)
	{
	#system("explorer $logpath");	
	} 
