@echo off
tsbuild.pl --remap 255,0,255=0 --trans 0 --outdir ..\data --tiled --format rgb256 ..\art\mapdata\maps.raw ..\art\mapdata\regular.png ..\art\mapdata\newbldgs.bmp ..\art\*.png 
conv16bit.pl ..\art\16bit\*.png
wav2bin.pl ..\art\*.wav
makefont_direct.pl dpcomic.ttf 16 0 ..\data\font_noaa.bin
