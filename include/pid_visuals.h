#pragma once

void rng();
void addPIDNode(double up, double ui, double ud);
void updateBoatPosition(double position);
void updateReference(double new_reference);
void graphInit(int* argc,
        char** argv,
        double b_max,
        double b_min,
        void (*specialKeyPressed)(unsigned char, int, int),
        char* log_path);
