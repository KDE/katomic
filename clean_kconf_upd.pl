#!/usr/bin/perl -w

# I wrote this little script to make kconf_update "forget" that it did apply .upd file to katomicrc
# and also some other nifty things i used while testing
#
# It contains some hardcoded paths, need to be adapted. I commit it because i might need it to do additional testing
# if something fails.
#
# It can be safely removed after sometime I guess :)
#     - dimsuz

use File::Copy;

copy("/home/archuser/tmp/katomicrc", "/home/archuser/.kde4/share/config/katomicrc") or die $!;

my $file1="/home/archuser/.kde4/share/config/kconf_updaterc";
my $file2="/home/archuser/.kde4/share/config/kconf_updaterc_tmp";
open FILE, "<$file1";
open FILE1, ">$file2";

my $groupstart =0;
while(<FILE>)
{
	chomp;
	if(not $groupstart and $_ =~ /^\[katomic.*/)
	{
		$groupstart=1;
	}
	elsif($groupstart and $_ =~/^\[/)
	{
		$groupstart = 0;
	}

	if(not $groupstart)
	{
		print FILE1 $_."\n"
	}
}
close FILE1;
close FILE;

move($file2,$file1);

#copy("/home/archuser/kdesvn/kdegames/katomic/katomic-levelset.upd", "/home/archuser/.kde4/share/apps/kconf_update/") or die $!;
#copy("/home/archuser/kdesvn/kdegames/katomic/katomic-levelset-upd.pl", "/home/archuser/.kde4/share/apps/kconf_update/") or die $!;
