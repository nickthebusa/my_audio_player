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

dependencies:

Unix:
sudo apt install build-essential libncursesw5-dev   # Debian/Ubuntu
sudo dnf install gcc make ncurses-devel              # Fedora
sudo pacman -S base-devel ncurses                    # Arch
brew install ncurses                                 # macOS

Windows:
download gcc compiler
    - via msys2: https://www.msys2.org/
    - via mingw-w64: https://www.mingw-w64.org/
    - using chocolatey: choco install mingw 

install git (for downloading the pdcursesmod library)
    - installer: https://git-scm.com/install/
    - winget: winget install --id Git.Git -e --source winget
    - chocolatey: choco install git -y
