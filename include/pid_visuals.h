#pragma once

void rng();
void addPIDNode(double up, double ui, double ud);
void setBoatPosition(double position, double min_val, double max_val);
void graphInit(int* argc, char** argv, char* log_path);
