#!/usr/bin/perl

#maketar.pl generates arplib.tar.bz2

$arpdir = "/home/arp/stm/dfu/stmdfu/";
@arpfiles = ("arperr.c",
		"arperr.h",
		"arpinit.c",
		"arpinit.h",
		"arpint.c",
		"arpint.h",
		"arplibs.h",
		"arp.r8.ld",
		"arp.rb.ld",
		"arp.rc.ld",
		"arpsample.c",
		"arpsample.h",
		"stm32f10x_conf.h"
);

$tararg = "";

foreach $arpfile (@arpfiles)
{
	$tararg = $tararg.$arpfile." ";
}

$tarcmd = "cd $arpdir && tar -cjf arplib.tar.bz2 $tararg && cd -";

print $tarcmd;
print "\n";
`$tarcmd`;
