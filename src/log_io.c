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

void writePIDValues(double up, double ui, double ud, double pos, double ref, double dt, FILE* fp) {
    fprintf(fp, "%f %f %f %f %f\n", up, ui, ud, pos, dt, ref);
}

bool hasNext(FILE* fp) {
    return feof(fp) == 0;
}

char* nextLine(FILE* fp, char* buffer[]) {
    fgets(buffer, sizeof(buffer), fp);
    return buffer;
}

FILE* openFile(char file_name[], char* mode) {
    FILE* f = fopen(file_name, mode);
    if (f == NULL) {
        printf("Error opening logfile\n");
        exit(1);
    }
    return f;
}

void closeFile(FILE* fp) {
    fclose(fp);
}

