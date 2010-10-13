use strict;
use warnings;
use Data::Dumper;
use Getopt::Long;
use Image::Magick;
use POSIX;

my $format = 'rgb8_a5';
my $GFX_TEX_FORMAT = 0x1800; # rgb8_a5 format
my @palette;
my %colourToIndex;

sub nextPowerOfTwo($)
{
	my $dim = shift;
	my $dimlog2 = log($dim)/log(2);
	return pow(2, ceil($dimlog2));
}

sub texFormatSpec($)
{
	my $dim = shift;
	my $dimlog2 = log($dim)/log(2);
	return $dimlog2 - 3;
}

sub numColours()
{
	return 4 if ($format eq 'rgb4');
	return 16 if ($format eq 'rgb16');
	return 8 if ($format eq 'rgb8_a5');
	return 256 if ($format eq 'rgb256');
	die "unsupported format";
}

if ($#ARGV != 5)
{
	print "usage: makefont.pl fontname.ttf <pointsize> <antialias 0|1> <texture_width> metrics_file.bin texture_file.bin\n";
	exit 1;
}

open METRICS, ">$ARGV[4]" or die "couldn't open $ARGV[4] for output";
binmode(METRICS);
open TEX, ">$ARGV[5]" or die "couldn't open $ARGV[5] for output";
binmode(TEX);

my $NUMBUTTONS = 11;

my $textureWidth = int($ARGV[3]);

my $image = new Image::Magick->new();
$image->Set(size=>"${textureWidth}x1024");
$image->ReadImage('xc:black');

my $str = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~';
my $params = {
	text => $str,
	font => $ARGV[0],
	pointsize => int($ARGV[1]),
	fill => 'white',
	antialias => int($ARGV[2])
};
my ($x_ppem, $y_ppem, $ascender, $descender, $width, $height, $max_advance) = $image->QueryFontMetrics(%$params);
my $lineHeight = $ascender - $descender;
my $curX = 0;
my $curY = $ascender;

print METRICS pack('v', $lineHeight);

for (my $i = 0; $i < length($str); ++$i)
{
	$params->{x} = $curX;
	$params->{y} = $curY;
	$params->{text} = substr($str, $i, 1);
	my $w = ($image->QueryFontMetrics(%$params))[4];
	if ($curX + $w >= $textureWidth)
	{
		$curX = 0;
		$curY += $lineHeight;
		$params->{x} = $curX;
		$params->{y} = $curY;
	}
	print METRICS pack('vvv', $curX, $curY, $w);
	$image->Annotate(%$params);
	$curX += $w;
}

$curY -= $ascender;
for (my $i = 0; $i < $NUMBUTTONS; $i++)
{
	my $buttons = new Image::Magick->new();
	$buttons->Read('buttons.png');
	my $buttonWidth = $buttons->Get('width') / $NUMBUTTONS;
	my $buttonHeight = $buttons->Get('height');
	my $geomStr = "${buttonWidth}x${buttonHeight}+" . ($i*$buttonWidth) . "+0";
	$buttons->Crop(geometry => $geomStr);
	$buttons->Write("b$i.png");

	if ($curX + $buttonWidth >= $textureWidth)
	{
		$curX = 0;
		$curY += $lineHeight;
	}
	print METRICS pack('vvv', $curX, $curY, $buttonWidth);
	$image->Composite(image => $buttons, x => $curX, y => $curY + ($lineHeight - $buttonHeight) / 2);
	$curX += $buttonWidth;
}

if ($curX != 0)
{
	$curY += $lineHeight;
}
my $finalTextureHeight = nextPowerOfTwo($curY);
$image->Crop(geometry => "${textureWidth}x$finalTextureHeight");
$image->Quantize(colors=>16);
$image->Write('test.png');

$GFX_TEX_FORMAT |= texFormatSpec($textureWidth) << (20 - 16);
$GFX_TEX_FORMAT |= texFormatSpec($finalTextureHeight) << (23 - 16);
print TEX pack('v', $GFX_TEX_FORMAT);
my $texData = '';

for (my $y = 0; $y < $finalTextureHeight; $y++)
{
	for (my $x = 0; $x < $textureWidth; $x++)
	{
		my @rgb = $image->GetPixels(width => 1, height => 1, x => $x, y => $x, normalize => 'true');
		die "expecting monochrome data" if ($rgb[0] != $rgb[1] || $rgb[0] != $rgb[2]);
		my $alpha = int($rgb[0] * 31 + 0.5);
		die "bad conversion" if ($alpha < 0 || $alpha > 31);
		$texData .= pack('C', (1 << 5) | $alpha);
	}
}

# output a trival palette with black as colour 0, white as rest; we use he 5 bits of alpha
# to encode the antialiasing info in the font data
for (my $i = 0; $i < numColours(); $i++)
{
	if ($i == 0)
	{
		print TEX pack ('v', 0x8000);
	}
	else
	{
		print TEX pack ('v', 0xffff);
	}
}

print TEX $texData;

close METRICS;
close TEX;
print "generated metrics file: $ARGV[4]\n";
print "generated font texture: $ARGV[5] (${textureWidth}x$finalTextureHeight)\n";

=pod
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
$image->Set(size=>"${ARGV[3]}x1024");
$image->ReadImage('xc:white');
$image->Annotate(%$params);
$image->Composite(image => $buttons, x => $buttonLocation, y => ($ad - $buttonHeight) / 2);
$image->Quantize(colors=>32);

$image->Write('temp.png');

open OUT, ">$ARGV[4]" or die "couldn't open $ARGV[4] for output";
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
=cut
