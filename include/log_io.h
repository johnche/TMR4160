#pragma once
#include <stdio.h>
#include <stdbool.h>

void writeHeaderValues(FILE* fp, double kp, double ki, double kd, int dt,
        double pscale, double iscale, double dscale);
void writePIDValues(FILE* fp, double up, double ui, double ud, double pos, double ref);
char* nextLine(FILE* fp, char* buffer[], int bufferlen);
FILE* openFile(char file_name[], char* mode);
void closeFile(FILE* fp);

