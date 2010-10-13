use strict;
use warnings;
use Data::Dumper;
use Getopt::Long;
use Image::Magick;
use File::Spec;

my @transes;
my @remaps;
my $outdir = '.';
my $paletteFileOpt = 'mainpal.bin';
my $tiled = 0;
my $format = 'rgb256';
exit 1 if not GetOptions("remap=s" => sub
	{
		my $name = shift;
		my $val = shift;

		$val =~ m/\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*=\s*(\d+)\s*/;
		push @remaps, [ $1, $2, $3, $4 ];
	}, "trans=s" => \@transes,
	"outdir=s" => \$outdir,
	"palette=s" => \$paletteFileOpt,
	"tiled" => \$tiled,
	"format=s" => \$format);

my $paletteFile = "$outdir\\$paletteFileOpt";

if ($#ARGV < 2)
{
	print "\n" . 
		  "tsbuild.pl [options] maps.raw tileset.bmp unplacedtiles.bmp [objects.bmp]...\n" .
	      "\n" .
		  "where options are:\n" .
		  "\n" .
		  "--remap r,g,b=index     maps colour rgb to index in palette\n" .
		  "--palette filename      outputs palette to specified filename\n" .
		  "                        (default is mainpal.bin)\n" .
		  "--trans index           makes index of the palette transparent\n" .
		  "--outdir dir            directory to output .bin files to\n" .
		  "--tiled                 output as tiled\n" .
		  "--format <fmt>          output as specified format, one of: rgba,\n".
		  "                        rgb32_a3, rgb4, rgb16, rgb256, or rgb8_a5.\n".
		  "                        default = rgb16.\n".
		  "\n".
		  "maps.raw is in the format specified by Luvafair.tsd in TileStudio\n".
		  "the '.bmp' can be any image format that ImageMagick supports (most)\n".
		  "\n"
	;
	exit 0;
}

my @palette;
my %colourToIndex;

my $tileWidth = 32;
my $tileHeight = 32;
my $charactersPerTileX = ($tileWidth / 8);
my $charactersPerTileY = ($tileHeight / 8);


sub strForColours($$$)
{
	my $r = shift;
	my $g = shift;
	my $b = shift;
	return ($r >> 3) . '_' . ($g >> 3) . '_' . ($b >> 3);
}

sub numColours()
{
	return 4 if ($format eq 'rgb4');
	return 16 if ($format eq 'rgb16');
	return 256 if ($format eq 'rgb256');
	die "unsupported format";
}

sub getIndexFor($$$)
{
	my $r = shift;
	my $g = shift;
	my $b = shift;

	my $str = strForColours($r, $g, $b);
	if (defined $colourToIndex{$str})
	{
		return $colourToIndex{$str};
	}

	my $index = -1;
	for (my $i = 0; $i < numColours(); ++$i)
	{
		if (not defined $palette[$i])
		{
			$index = $i;
			last;
		}
	}
	die "ran out of colours" if $index == -1;
	$colourToIndex{$str} = $index;
	push @palette, $str;
	return $index;
}

sub getIndexForNormalized($$$)
{
	my $r = shift;
	my $g = shift;
	my $b = shift;

	return getIndexFor(int($r * 255 + 0.5), int($g * 255 + 0.5), int($b * 255 + 0.5));
}

sub getUsedColours()
{
	my $count = 0;
	for (my $i = 0; $i < numColours(); ++$i)
	{
		if (defined $palette[$i])
		{
			++$count;
		}
	}
	return $count;
}

sub outputTiles($)
{
	my $tilefile = shift;

	my $tiles = Image::Magick->new();
	$tiles->Read($tilefile);
	die "expecting tile width to be $tileWidth" if $tiles->Get('width') != $tileWidth;
	die "expecting tile height to be mod $tileHeight" if $tiles->Get('height') % $tileHeight != 0;

	my $w = $tiles->Get('width');
	my $h = $tiles->Get('height');
	my $beforeColours = getUsedColours();
	print "  including data from $tilefile...";
	for (my $y = 0; $y < $h / 8; $y++)
	{
		for (my $x = 0; $x < $w / 8; $x++)
		{
			for (my $ty = 0; $ty < 8; $ty++)
			{
				for (my $tx = 0; $tx < 8; $tx++)
				{
					my $pixelX = $tx + ($x * 8);
					my $pixelY = $ty + ($y * 8);
					my @rgb = $tiles->GetPixels(width => 1, height => 1, x => $pixelX, y => $pixelY, normalize => 'true');
					print OUT pack('C', getIndexForNormalized($rgb[0], $rgb[1], $rgb[2]));
				}
			}
		}
	}
	my $afterColours = getUsedColours();
	my $newColours = $afterColours - $beforeColours;
	print "($newColours new colours)\n";
}

sub outputSprite($)
{
	my $file = shift;

	my $image = Image::Magick->new();
	$image->Read($file);

	my ($a,$b,$outfile) = File::Spec->splitpath($file);
	$outfile =~ s/(.*\.).*$/$outdir\\$1bin/;
	open OUT, ">$outfile" or die "couldn't open $outfile";
	binmode(OUT);

	my $w = $image->Get('width');
	my $h = $image->Get('height');
	die "expecting tile width to be mod 8" if $w % 8 != 0;
	die "expecting tile height to be mod 8" if $h % 8 != 0;

	my $beforeColours = getUsedColours();
	print "converting $file to $outfile...";
	for (my $y = 0; $y < $h / 8; $y++)
	{
		for (my $x = 0; $x < $w / 8; $x++)
		{
			for (my $ty = 0; $ty < 8; $ty++)
			{
				for (my $tx = 0; $tx < 8; $tx++)
				{
					my $pixelX = $tx + ($x * 8);
					my $pixelY = $ty + ($y * 8);
					my @rgb = $image->GetPixels(width => 1, height => 1, x => $pixelX, y => $pixelY, normalize => 'true');
					print OUT pack('C', getIndexForNormalized($rgb[0], $rgb[1], $rgb[2]));
				}
			}
		}
	}
	my $afterColours = getUsedColours();
	my $newColours = $afterColours - $beforeColours;
	print "($newColours new colours)\n";

	close OUT;
}

foreach my $remap (@remaps)
{
	my $str = strForColours($remap->[0], $remap->[1], $remap->[2]);
	$colourToIndex{$str} = int($remap->[3]);
	$palette[$remap->[3]] = $str;
}

open OUT, ">$outdir\\tiles.bin" or die "couldn't open $outdir\\tiles.bin";
binmode(OUT);
print "writing tile data to $outdir\\tiles.bin...\n";
outputTiles($ARGV[1]);
outputTiles($ARGV[2]);
close OUT;


my @spriteFiles;
for (my $i = 3; $i <= $#ARGV; $i++)
{
	my @globbed = glob($ARGV[$i]);
	push @spriteFiles, @globbed;
}

foreach my $spriteFile (@spriteFiles)
{
	outputSprite($spriteFile);
}

open MAPS, "$ARGV[0]" or die "couldn't open $ARGV[0]";

my $curMapName;
my $curWidth;
my $curHeight;
my $numTiles;
my @curTiles;
my @curFlags;
while (<MAPS>)
{
	my $line = $_;
	chomp($line);
	if ($line =~ m/numtiles\s*=\s*(\d+)/)
	{
		$numTiles = $1;
	}
	elsif ($line =~ m/map (.*)/)
	{
		$curMapName = $1;
	}
	elsif (defined $curMapName)
	{
		if ($line =~ m/width\s*=\s*(\d+)/)
		{
			$curWidth = $1;
		}
		elsif ($line =~ m/height\s*=\s*(\d+)/)
		{
			$curHeight = $1;
		}
		elsif ($line =~ m/tiles\s*=\s*(.*)/)
		{
			@curTiles = split(/,/, $1);
		}
		elsif ($line =~ m/flags\s*=\s*(.*)/)
		{
			@curFlags = split(/,/, $1);
		}
		elsif ($line =~ m/endmap/)
		{
			open OUT, ">$outdir\\$curMapName.bin" or die "couldn't open $outdir\\$curMapName.bin";
			binmode(OUT);
			print "writing map data to $outdir\\$curMapName.bin...\n";
			print OUT pack('vv', $curWidth, $curHeight);
			print OUT pack('v', $numTiles);
			print OUT pack(sprintf('C%d', $curWidth*$curHeight), @curFlags);
			for (my $y = 0; $y < $curHeight * $charactersPerTileY; ++$y)
			{
				for (my $x = 0; $x < $curWidth * $charactersPerTileX; ++$x)
				{
					my $largeIndexX = int($x / $charactersPerTileX);
					my $largeIndexY = int($y / $charactersPerTileY);

					my $subIndexX = $x % $charactersPerTileX;
					my $subIndexY = $y % $charactersPerTileY;

					# data from TileStudio for maps is 1 based (!)
					my $largeTileIndex = $curTiles[$largeIndexY * $curWidth + $largeIndexX] - 1;

					my $charactersPerTile = $charactersPerTileX * $charactersPerTileY;

					my $characterTileIndex = ($largeTileIndex * $charactersPerTile) + ($subIndexY * $charactersPerTileX) + $subIndexX;
					print OUT pack('v', $characterTileIndex);
				}
			}
			close OUT;
			$curMapName = undef;
		}
	}
}
close MAPS;


my $usedColours = 0;
print "outputting '$paletteFile'...";
open OUT, ">$paletteFile" or die "couldn't open $paletteFile for output";
binmode(OUT);
for (my $i = 0; $i < numColours(); $i++)
{
	my $colour = 0xffff;
	if (defined $palette[$i])
	{
		$palette[$i] =~ m/(\d+)_(\d+)_(\d+)/;
		#print(sprintf("%d %d %d\n", $1, $2, $3));
		$colour = 0x8000 | ($3 << 10) | ($2 << 5) | $1;
		#print sprintf("%x\n", $colour);
		$usedColours++;
	}
	print OUT pack('v', $colour);
}
close OUT;
print "$usedColours colours\n";

