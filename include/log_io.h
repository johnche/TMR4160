#pragma once
#include <stdio.h>
#include <stdbool.h>

void writePIDValues(double up, double ui, double ud, double pos, double ref, double dt, FILE* fp);
bool hasNext(FILE* fp);
char* nextLine(FILE* fp, char* buffer[]);
FILE* openFile(char file_name[], char* mode);
void closeFile(FILE* fp);

