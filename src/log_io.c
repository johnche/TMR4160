#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "log_io.h"

//FILE *f = fopen("file.txt", "w");
//if (f == NULL)
//{
//    printf("Error opening file!\n");
//    exit(1);
//}
//
///* print some text */
//const char *text = "Write this to the file";
//fprintf(f, "Some text: %s\n", text);
//
///* print integers and floats */
//int i = 1;
//float py = 3.1415927;
//fprintf(f, "Integer: %d, float: %f\n", i, py);
//
///* printing single chatacters */
//char c = 'A';
//fprintf(f, "A character: %c\n", c);
//
//fclose(f);

void writeHeaders() {
}

void writeHeaderValues(FILE* fp,
        double kp, double ki, double kd, int dt,
        double pscale, double iscale, double dscale){
    fprintf(fp, "%f %f %f %d %f %f %f\n", kp, ki, kd, dt, pscale, iscale, dscale);
}

void writePIDValues(FILE* fp, double up, double ui, double ud, double pos, double ref) {
    fprintf(fp, "%f %f %f %f %f\n", up, ui, ud, pos, ref);
}

bool hasNext(FILE* fp) {
    return feof(fp) == 0;
}

char* nextLine(FILE* fp, char* buffer[], int bufferlen) {
    fgets(buffer, bufferlen, fp);
    return buffer;
}

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

void closeFile(FILE* fp) {
    fclose(fp);
    printf("Logfile closed\n");
}

