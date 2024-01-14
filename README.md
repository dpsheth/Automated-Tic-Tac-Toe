This repository runs on the idea of making automated Tic-Tac-Toe rounds that run for 100 iterations of different games and have it be run through an ESP32 while displaying said results of all games on an LCD display. Logic for game's rules and such are mainly developed in the Arduino file while the game itself is played through a Google Cloud Protocol instance to play as X and O respectively. 
What is not shown here is the CRON job operation in specific, and as so, it will be listed below this paragraph. Every round's results save on a file and also takes time to start up again

 */4 * * * * /bin/bash /home/darshilsheth491/tscript.sh 

 This cron job would run roughly a few minutes after each round is made, witha  file being generated each time through the script.

This project was done with some assistance by Nicholas Hoang and Jake Petersen in developing the full structure of the game. 
