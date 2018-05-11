#pragma once

void renderText(char s[], double x, double y);
int openGLinit(int* argc,
        char** argv,
        void (*keyPressed)(unsigned char, int, int),
        void (*specialKeyPressed)(unsigned char, int, int));
