#!/bin/bash

clear

echo "Welcome to my game!"
echo "you're mission is to save youe fellow crew member that has been abducted by aliens."
echo "You can shoot them by pressing the space bar and you move by using the arrow keys."
echo "Make sure that you do not hit any of the walls or aliens otherwise you will die, Good Luck!."

g++ -c -std=c++11 src/*.cpp
g++ -o Star *.o -lSDL2 -lSDL2_image
./Star
