sox "click03.wav" -r 8000 -v 7 -b -c 1 ..\art\click.wav 
sox "\WINDOWS\Media\Windows XP Balloon.wav" -r 8000 -v 4 -b -c 1 ..\art\balloon.wav 
sox "\WINDOWS\Media\Windows XP Minimize.wav" -r 8000 -v 5 -b -c 1 ..\art\menudown.wav 
sox "\WINDOWS\Media\Windows XP Restore.wav" -r 8000 -v 5 -b -c 1 ..\art\menuup.wav 
sox place_building.wav -r 8000 -b -c 1 ..\art\place_building.wav
sox "\WINDOWS\Media\Windows XP Hardware Insert.wav" -r 8000 -b -c 1 ..\art\building_finished.wav
::sox clock.wav -r 8000 -b -c 1 ..\art\clock.wav
sox 18400_inferno_waterxplo.wav -r 8000 -b -c 1 ..\art\destroy.wav
sox error.wav -r 8000 -b -c 1 ..\art\error.wav
sox dingling.wav -r 8000 -b -c 1 ..\art\research_complete.wav
sox winner2.wav -r 8000 -b -c 1 ..\art\win.wav
sox loser3.wav -r 8000 -b -c 1 ..\art\lose.wav
sox musical078_2.wav -r 8000 -b -c 1 ..\art\war.wav
