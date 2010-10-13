use strict;
use warnings;
use Audio::Wav;
use Data::Dumper;

my @allWavs;
for (my $i = 0; $i <= $#ARGV; $i++)
{
	my @globbed = glob($ARGV[$i]);
	push @allWavs, @globbed;
}

foreach my $wavName (@allWavs)
{
	my $wav = new Audio::Wav;
	my $read = $wav->read($wavName);
	my $outname = $wavName;
	$outname =~ s/.*\\(.*)\.wav/..\\data\\$1.bin/;
	print "converting $wavName to $outname...";

	my $details = $read->details();

	die "only done mono" if $details->{channels} != 1;
	die "only do pcm8/pcm16" if $details->{bits_sample} != 8 && $details->{bits_sample} != 16;

	print "($details->{sample_rate}Hz, $details->{bits_sample}bit)\n";

	#print Dumper($details);

	die "length bigger than s16" if ($details->{data_length} >= 32768);

	open OUT, ">$outname" or die "couldn't open $outname";
	binmode(OUT);
	print OUT pack('S', $details->{sample_rate});
	print OUT pack('S', 0); # 0 = mono, 1 = stereo
	print OUT pack('S', $details->{bits_sample});
	print OUT pack('S', $details->{data_length});


	if ($details->{bits_sample} == 8)
	{
		for (my $j = 0; $j < $details->{data_length}; $j++)
		{
			my @data = $read->read();
			#print Dumper($data[0]);
			print OUT pack('c', $data[0]);
		}
	}
	elsif ($details->{bits_sample} == 16)
	{
		for (my $j = 0; $j < $details->{data_length}; $j++)
		{
			my @data = $read->read();
			print OUT pack('c', $data[0]);
		}
	}
	else
	{
		die "bad";
	}

	close OUT;
}
