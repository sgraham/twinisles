# a simple font spitter-outer.
# file format in lg/font.h

use strict;
use warnings;
use Data::Dumper;
use Getopt::Long;
use Image::Magick;

if ($#ARGV != 3)
{
	print "usage: makefont.pl fontname.ttf <pointsize> <antialias 0|1> output_file.bin\n";
	exit 1;
}

my $NUMBUTTONS = 11;

my $temp = new Image::Magick->new();
$temp->ReadImage('xc:white');

my $str = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~';
my $params = {
	x => 0,
	text => $str,
	font => $ARGV[0],
	pointsize => int($ARGV[1]),
	fill => 'black',
	antialias=>int($ARGV[2])
};

my ($x_ppem, $y_ppem, $ascender, $descender, $width, $height, $max_advance) = $temp->QueryFontMetrics(%$params);
my $ad = $ascender - $descender;
$params->{y} = $ascender;

my $buttonLocation = 0;
my $buttons = new Image::Magick->new();
$buttons->Read('buttons.png');
my $buttonHeight = $buttons->Get('height');
my $buttonWidth = $buttons->Get('width');
if ($buttonHeight > $ad)
{
	print "error: height of font is less than height of buttons\n";
	exit 1;
}
else
{
	$buttonLocation = $width;
	$width += $buttonWidth;
}

my $image = new Image::Magick->new();
$image->Set(size=>"${width}x${ad}");
$image->ReadImage('xc:white');
$image->Annotate(%$params);
$image->Composite(image => $buttons, x => $buttonLocation, y => ($ad - $buttonHeight) / 2);
$image->Quantize(colors=>16);

$image->Write('temp.png');

open OUT, ">$ARGV[3]" or die "couldn't open $ARGV[3] for output";
binmode(OUT);

print OUT pack('vv', $width, $ad);

my $curXPos = 0;
for (my $i = 0; $i < length($str); $i++)
{
	$params->{text} = substr($str, $i, 1);
	my $charWidth = ($temp->QueryFontMetrics(%$params))[4];
	die "charwidth > 255?" if $charWidth > 255;
	print OUT pack('v', $curXPos);
	$curXPos += $charWidth;
}
for (my $i = 0; $i < $NUMBUTTONS; $i++)
{
	print OUT pack('v', $curXPos);
	$curXPos += $buttonWidth / $NUMBUTTONS;
}

for (my $i = 0; $i < length($str); $i++)
{
	$params->{text} = substr($str, $i, 1);
	my $charWidth = ($temp->QueryFontMetrics(%$params))[4];
	die "charwidth > 255?" if $charWidth > 255;
	print OUT pack('C', $charWidth);
}
die "button width isn't a multiple of how many buttons there are" if $buttonWidth % $NUMBUTTONS != 0;
for (my $i = 0; $i < $NUMBUTTONS; $i++)
{
	print OUT pack('C', $buttonWidth / $NUMBUTTONS);
}

for (my $y = 0; $y < $ad; $y++)
{
	for (my $x = 0; $x < $width; $x++)
	{
		my $clr = int(($image->GetPixels(width => 1, height => 1, x => $x, y => $y, normalize => 'true'))[0] * 255 + 0.5);
		die "bad conversion" if $clr < 0 || $clr > 255;
		print OUT pack('C', (255 - $clr));
	}
}

close OUT;

