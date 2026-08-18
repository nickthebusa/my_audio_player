This is a basic terminal audio player.
There are 4 main parts of the program, a directory explorer, a queue, a player, and a command prompt.
To move between the directory explorer and the queue, use the tab key.
There is also a help window that pops up when the user presses the ? key.
To quit the program, press : to enter the comand mode the type q and press enter.
There are not many commands in the command mode, the only other one is to type files to see the current width of the
directory explorer, or type files 'width' to set the width of the directory explorer. (ex: files 35)
The directory explorer allows the user to explore the directory they are currently in, also allows them to go into 
new directories, and into the parent directories. In each directory, the folders inside will be at the top of the explorer,
then any audio files in the directory will be listed afterwards. From the directory explorer, the user can select a file
and add it to the queue, or using CTRL+a add all the audio files to the queue.
Single audio files in the queue can be removed (by pressing r),or the entire queue can be cleared (by pressing CTRL+r).
Each entry in the queue can be moved up or down using SHIFT+UP or SHIFT+DOWN. (and the vim motions j and k with shift).
The player is a section at the bottom that shows the file name, the current/total time,and whether or not the song is paused.
Pressing space will toggle playing the song or pausing it, n will go to the next song, and p will go to the previous song.
You can also seek forward and back using < and >.

To run the program I named the executable "my_audio_player", then copied it to my binary files on linux.
So you can download the code, run make then run the executable. (or move it to your path to run globally)
