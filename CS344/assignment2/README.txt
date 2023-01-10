To compile my program:

1. The command to compile the program: gcc --std=gnu99 -o movies_by_year movies_by_year.c

2. Enter the executable in the command line: ./movies along with the name of the csv file
 -Example: ./movies_by_year movies_sample_1.csv

3. Options for what you want to do within the program will appear. You will need to provide the csv file in the command line.

Note: For whatever unholy reason, vscode and <dirent.h> are NOT friends. This program runs just fine on the server but it will have to be done on Mobaxterminal or another text editor.