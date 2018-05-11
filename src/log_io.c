#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "log_io.h"

/*
 * Backend for all IO operations.
 * These are used for logging controller states and parameters,
 * but also reading the same logs.
 */

/*
 * Writes the first line in a log which should contain initializer
 * parameters such as samplerate, gain constants and graph scaling.
 */
void writeHeaderValues(FILE* fp,
        double kp, double ki, double kd, int dt,
        double pscale, double iscale, double dscale){
    fprintf(fp, "%f %f %f %d %f %f %f\n", kp, ki, kd, dt, pscale, iscale, dscale);
}

/*
 * Writes the state of the control system
 */
void writePIDValues(FILE* fp, double up, double ui, double ud, double pos, double ref) {
    fprintf(fp, "%f %f %f %f %f\n", up, ui, ud, pos, ref);
}

/*
 * Returns next line found in the file from fp.
 * Returns NULL if there is no next line.
 */
char* nextLine(FILE* fp, char* buffer[], int bufferlen) {
    fgets(buffer, bufferlen, fp);
    return buffer;
}

/*
 * Opens a filepointer.
 * If a path is provided, it will open up the file with read access
 * at this path.
 * Otherwise, it will create a new file which it will have write
 * access to. This file will be named
 * "pidlog_[year month day hour min sec].txt".
 */
FILE* openFile(char file_name[], char* mode) {
    FILE* f;
    if (file_name)
        f = fopen(file_name, mode);
    else {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char* f_name[64];
        sprintf(f_name, "pidlog_%d%d%d%d%d%d.txt",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        f = fopen(f_name, mode);
    }

    if (f == NULL) {
        printf("Error opening logfile\n");
        exit(1);
    }
    return f;
}

/*
 * Closes file (important!)
 * Not closing can possibly corrupt file.
 */
void closeFile(FILE* fp) {
    fclose(fp);
    printf("Logfile closed\n");
}

