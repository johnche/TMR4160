#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "linked_list.h"
#include "glplot.h"

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
    return f_min + f*(f_max - f_min);
}

double scaleValue(double scale_min, double scale_max, double value_max, double value) {
    double percentage = value / value_max;
    return scale_min + percentage*(scale_max - scale_min);
}

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 800;
double sum_bottom = 7.6;
double kp_bottom = 5.1;
double ki_bottom = 2.6;
double kd_bottom = 0.1;
double g_width = 15;
double g_height = 2.4;
double x_left = 0.1;
double kp_r;
double ki_r;
double kd_r;
double sum_r;
LinkedList* kp_data;
LinkedList* ki_data;
LinkedList* kd_data;


double step = 0.1;
void renderLLData() {
    Node* current[] = {kp_data->root, ki_data->root, kd_data->root};
    if (current[0] == NULL || current[0]->next == NULL)
        return;

    double counter = 0;
    glBegin(GL_LINES);
    glLineWidth(1.0);
    while (current[0]->next != NULL) {
        //glColor3f(1, 0, 0);
        glColor3ub(71, 144, 48);
        glVertex3f(x_left + 0.1 + counter-step, current[0]->data, 0);
        glVertex3f(x_left + 0.1 + counter, current[0]->next->data, 0);
        current[0] = current[0]->next;

        //glColor3f(0, 1, 0);
        glColor3ub(37, 87, 107);
        glVertex3f(x_left + 0.1 + counter-step, current[1]->data, 0);
        glVertex3f(x_left + 0.1 + counter, current[1]->next->data, 0);
        current[1] = current[1]->next;

        //glColor3f(0, 0, 1);
        glColor3ub(162, 54, 69);
        glVertex3f(x_left + 0.1 + counter-step, current[2]->data, 0);
        glVertex3f(x_left + 0.1 + counter, current[2]->next->data, 0);
        current[2] = current[2]->next;

        if (counter > g_width - 2*step) {
            leftLLPop(kp_data);
            leftLLPop(ki_data);
            leftLLPop(kd_data);
        }
        else
            counter += step;
    }
    glEnd();
    printf("counter %f\n", counter);

}

void rng() {
    addLLNode(kp_data, fRand(kp_bottom, kp_bottom + g_height));
    addLLNode(ki_data, fRand(ki_bottom, ki_bottom + g_height));
    addLLNode(kd_data, fRand(kd_bottom, kd_bottom + g_height));
    printf("kp %f, ki %f, kd %f\n", kp_data->tail->data, ki_data->tail->data, kd_data->tail->data);
    usleep(50000);
}

void addPIDNode(double Kp, double Ki, double Kd) {
    addLLNode(kp_data, scaleValue(kp_bottom, kp_bottom + g_height, 200, Kp));
    addLLNode(ki_data, scaleValue(ki_bottom, ki_bottom + g_height, 200, Ki));
    addLLNode(kd_data, scaleValue(kd_bottom, kd_bottom + g_height, 200, Kd));
}

void drawLayout() {
    glRectf(0.1, 10.5, 15, 11.0);

    glColor3ub(251, 241, 199);
    glRectf(x_left, sum_bottom, x_left + g_width, sum_bottom + g_height);
    glRectf(x_left, kp_bottom, x_left + g_width, kp_bottom + g_height);
    glRectf(x_left, ki_bottom, x_left + g_width, ki_bottom + g_height);
    glRectf(x_left, kd_bottom, x_left + g_width, kd_bottom + g_height);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(x_left, sum_r, 0);
    glVertex3f(x_left + g_width, sum_r, 0);

    glVertex3f(x_left, kp_r, 0);
    glVertex3f(x_left + g_width, kp_r, 0);

    glVertex3f(x_left, ki_r, 0);
    glVertex3f(x_left + g_width, ki_r, 0);

    glVertex3f(x_left, kd_r, 0);
    glVertex3f(x_left + g_width, kd_r, 0);
    glEnd();
    //glRectf(0.5, 5, 15, 7);
}

void drawAxis() {
    glBegin(GL_LINES);
    // x-axis
    glLineWidth(1.0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);   glVertex3f(10.0f, 0.0f, 0.0f);
    // y-axis
    glLineWidth(1.0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);   glVertex3f(0.0f, 10.0f, 0.0f);
    glEnd();
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
    //glClearColor(0.8f, 0.8f, 0.6f, 1.f);
    glClearColor(124/255.0f, 111/255.0f, 100/255.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-9.23f, -5.75f, -10.0f);

    drawLayout();
    drawAxis();
    renderLLData();
    //drawGraph();

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
    kp_data = newLinkedList();
    ki_data = newLinkedList();
    kd_data = newLinkedList();
    kp_r = (2*kp_bottom + g_height)/2;
    ki_r = (2*ki_bottom + g_height)/2;
    kd_r = (2*kd_bottom + g_height)/2;
    sum_r = (2*sum_bottom + g_height)/2;

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
