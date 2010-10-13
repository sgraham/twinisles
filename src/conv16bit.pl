# a simple font spitter-outer.
# file format in lg/font.h

use strict;
use warnings;
use Data::Dumper;
use Getopt::Long;
use Image::Magick;

if ($#ARGV < 0)
{
	print "usage: conv1bit.pl <files...>\n";
	exit 1;
}

my @files;
for (my $i = 0; $i <= $#ARGV; $i++)
{
	my @globbed = glob($ARGV[$i]);
	push @files, @globbed;
}

foreach my $file (@files)
{
	my $outfile = $file;
	$outfile =~ s/.*\\(.*)\.png/..\\data\\$1.bin/;
	print "converting 16bit $file to $outfile...";

	open OUT, ">$outfile" or die "couldn't open $outfile for output";
	binmode(OUT);

	my $image = new Image::Magick->new();
	$image->ReadImage($file);
	my $w = $image->Get('width');
	my $h = $image->Get('height');
	print "(${w}x${h})\n";
	for (my $y = 0; $y < $h; $y++)
	{
		for (my $x = 0; $x < $w; $x++)
		{
			my @clr = $image->GetPixels(width => 1, height => 1, x => $x, y => $y, normalize => 'true');
			my $r = int($clr[0] * 31 + 0.5);
			my $g = int($clr[1] * 31 + 0.5);
			my $b = int($clr[2] * 31 + 0.5);
			my $finalColour = 0x8000 | ($b << 10) | ($g << 5) | $r;
			print OUT pack('S', $finalColour);
		}
	}

	close OUT;
}
