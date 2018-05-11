#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "linked_list.h"
#include "gl_framework.h"

/*
 * Mostly boilerplate code here.
 * These are neccessary to run the glut/opengl framework.
 */

/*
 * Draw text s on GUI at position (x, y)
 */
void renderText(char s[], double x, double y) {
    glRasterPos2d(x, y);
    for (int i = 0; i < strlen(s); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}

void display() {
    // Background color
    glClearColor(124/255.0f, 111/255.0f, 100/255.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Move origin to bottom left of screen (hacky solution)
    glTranslatef(-9.23f, -5.75f, -10.0f);

    drawLayout();
    drawActors();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, (GLsizei) width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat) width / (GLfloat) height, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}


int openGLinit(int* argc,
        char** argv,
        void (*keyPressed)(unsigned char, int, int),
        void (*specialKeyPressed)(unsigned char, int, int)) {
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

