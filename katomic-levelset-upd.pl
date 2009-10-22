#!/usr/bin/perl -w

use File::Path qw(make_path);

#  WAS:
#  in katomicrc
#
#    [Highscores_user]
#    Level_1=15
#
#  NOW:
#  in $DATA_DIR/highscores:
#
#    [Highscores_user][default_levels]
#    Level1=15

my $prefix=`kde4-config --localprefix`;

chomp $prefix;

if(length($prefix) > 0)
{
    my $highscore_dir="$prefix/share/apps/katomic/";
    if(not -d $highscore_dir)
    {
        make_path($highscore_dir) or die $!;
    }
    open FILE, ">$highscore_dir/highscores" or die $!;

    my $firstHsGroup = 1;
    while( <> )
    {
        my $line = $_;
        chomp $line;
        if($line =~ /\[Highscores_/)
        {
            if(not $firstHsGroup)
            {
                print FILE "\n";
            }

            print FILE "$line\[default_levels\]\n";
            $firstHsGroup = 0;

            # make kconf_update delete group from rc file
            print "# DELETEGROUP $line\n";
        }
        elsif($line =~ /Level_([0-9]+)=([0-9]+)/)
        {
            print FILE "Level$1=$2\n";
        }
    }
}
