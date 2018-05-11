#pragma once
#include <stdio.h>

void rng();
void addPIDNode(double up, double ui, double ud);
void updateBoatPosition(double position);
void updateReference(double new_reference);
void togglePositionGraph();
void exitPIDVisuals();
void graphInit(int* argc,
        char** argv,
        double b_max,
        double b_min,
        void (*keyPressed)(unsigned char, int, int),
        void (*specialKeyPressed)(unsigned char, int, int),
        char* log_path);
