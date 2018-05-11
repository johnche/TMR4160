Dynamic Positioning (TMR4160)
=============================

This project is tested on GNU/Linux distributions. May not run on other..

Dependencies
-------------
For external dependencies, this project needs the openGL frameworks glut and GLEW in order to work.
Phidget22 is a dependency which is already included in the files.


Usage
-----
Please compile the project in a filesystem which supports symlinks.
To compile the project, run
    make
To run the project in live mode (the values in the brackets are mandatory)
    src/DynamicPositioning [Kp Ki Kd reference dt]
To run the project in log mode (replaying log)
    src/DynamicPositioning [path_to_log]

Hitting esc will exit the program, space will toggle position mode, and the left/right
arrows will decrease/increase the reference position.
