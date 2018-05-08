#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "linked_list.h"
#include "gl_framework.h"

#define foreach(item, array) \
    for(int keep = 1, \
            count = 0,\
            size = sizeof (array) / sizeof *(array); \
        keep && count != size; \
        keep = !keep, count++) \
      for(item = (array) + count; keep; keep = !keep)

#define len(array) (sizeof(array) / sizeof(array[0]))

double fRand(double f_min, double f_max) {
    double f = (double) rand() / RAND_MAX;
    return f*(f_max - f_min);
}

double scaleValue(double scale_min, double scale_max, double value_max, double value) {
    double percentage = value / value_max;
    return percentage*(scale_max - scale_min);
}

void renderText(char s[], double x, double y) {
    glRasterPos2i(x, y);
    for (int i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}

void keyPressed(unsigned char key, int x, int y) {
    printf("key %d\n", key);
    switch (key) {
        case 27:
            printf("Exiting..");
            exit(0);
            break;
    }
}

void specialKeyPressed(unsigned char key, int x, int y) {
    printf("specialkey %d\n", key);
    switch(key) {
        break;
    }
}

void display() {
    glClearColor(124/255.0f, 111/255.0f, 100/255.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-9.23f, -5.75f, -10.0f);

    // Draw actors
    drawAxis();
    drawLayout();
    setText();
    renderLLData();
    drawBoat();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, (GLsizei) width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat) width / (GLfloat) height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}


int openGLinit(int* argc, char** argv) {
    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 800;
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Window title");

    // Main loop callbacks
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);

    // Keyboard callbacks
    glutKeyboardFunc(keyPressed);
    glutSpecialFunc(specialKeyPressed);

    // Run glutmainloop in its own thread
    pthread_t glut_loop;
    if (pthread_create(&glut_loop, NULL, glutMainLoop, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    return 0;
}

