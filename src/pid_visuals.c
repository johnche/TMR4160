#include <GL/glew.h>
#include <GL/glut.h>
#include "linked_list.h"
#include "pid_visuals.h"
#include "gl_framework.h"

double boatline = 10.7;
double boatline_thickness = 0.4;
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
double kp_max = 0;
double ki_max = 0;
double kd_max = 0;
double sum_r;
double boat_position;
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
    while (current[0]->next != NULL && current[1]->next != NULL && current[2]->next != NULL) {
        // Sum graph
        glColor3ub(71, 144, 48);
        double pid_sum = current[0]->data + current[1]->data + current[2]->data;
        double next_pid_sum = current[0]->next->data + current[1]->next->data + current[2]->next->data;
        //printf("pid_sum %f, next_pid_sum %f\n", pid_sum, next_pid_sum);
        glVertex3f(x_left + 0.1 + counter-step, sum_bottom + pid_sum/3.0f, 0);
        glVertex3f(x_left + 0.1 + counter, sum_bottom + next_pid_sum/3.0f, 0);

        // Kp graph
        glColor3ub(71, 144, 48);
        glVertex3f(x_left + 0.1 + counter-step, kp_bottom + current[0]->data, 0);
        glVertex3f(x_left + 0.1 + counter, kp_bottom + current[0]->next->data, 0);
        current[0] = current[0]->next;

        // Ki graph
        glColor3ub(37, 87, 107);
        glVertex3f(x_left + 0.1 + counter-step, ki_bottom + current[1]->data, 0);
        glVertex3f(x_left + 0.1 + counter, ki_bottom + current[1]->next->data, 0);
        current[1] = current[1]->next;

        // Kd graph
        glColor3ub(162, 54, 69);
        glVertex3f(x_left + 0.1 + counter-step, kd_bottom + current[2]->data, 0);
        glVertex3f(x_left + 0.1 + counter, kd_bottom + current[2]->next->data, 0);
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

}

void rng() { // ONLY FOR TESTING
    addLLNode(kp_data, fRand(kp_bottom, kp_bottom + g_height));
    addLLNode(ki_data, fRand(ki_bottom, ki_bottom + g_height));
    addLLNode(kd_data, fRand(kd_bottom, kd_bottom + g_height));
}

//void showNewDataText(double up, double ui, double ud) {
//
//    renderText();
//}

void addPIDNode(double up, double ui, double ud) {
    if (up > kp_max)
        kp_max = up;
    if (ui > ki_max)
        ki_max = ui;
    if (ud > kd_max)
        kd_max = ud;

    double scaled_up = scaleValue(kp_bottom, kp_bottom + g_height, kp_max, up);
    double scaled_ui = scaleValue(ki_bottom, ki_bottom + g_height, ki_max, ui);
    double scaled_ud = scaleValue(kd_bottom, kd_bottom + g_height, kd_max, ud);
    if (scaled_up > kp_bottom + g_height)
        scaled_up = kp_bottom + g_height;
    if (scaled_ui > ki_bottom + g_height)
        scaled_ui = ki_bottom + g_height;
    if (scaled_ud > kd_bottom + g_height)
        scaled_ud = kd_bottom + g_height;
    addLLNode(kp_data, scaled_up);
    addLLNode(ki_data, scaled_ui);
    addLLNode(kd_data, scaled_ud);
}

void setBoatPosition(double position, double min_val, double max_val) {
    boat_position = x_left + scaleValue(x_left, x_left + g_width, max_val - min_val, position - min_val);
    //printf("boat pos %f\n", boat_position);
}

void drawBoat() {
    glColor3f(0, 0, 0);
    glRectf(boat_position - 0.05, boatline - 0.5, boat_position + 0.05, boatline + boatline_thickness + 0.4);
}

void drawLayout() {
    // Boat position rectangle
    glColor3ub(7, 102, 120);
    glRectf(x_left, boatline, x_left + g_width, boatline + boatline_thickness);

    // Rectangles for the four graphs
    glColor3ub(251, 241, 199);
    glRectf(x_left, sum_bottom, x_left + g_width, sum_bottom + g_height);
    glRectf(x_left, kp_bottom, x_left + g_width, kp_bottom + g_height);
    glRectf(x_left, ki_bottom, x_left + g_width, ki_bottom + g_height);
    glRectf(x_left, kd_bottom, x_left + g_width, kd_bottom + g_height);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    // Axis line for PID output
    glVertex3f(x_left, sum_r, 0);
    glVertex3f(x_left + g_width, sum_r, 0);

    // Axis line for the proportional term
    glVertex3f(x_left, kp_r, 0);
    glVertex3f(x_left + g_width, kp_r, 0);

    // Axis line for the integral term
    glVertex3f(x_left, ki_r, 0);
    glVertex3f(x_left + g_width, ki_r, 0);

    // Axis line for the derivative term
    glVertex3f(x_left, kd_r, 0);
    glVertex3f(x_left + g_width, kd_r, 0);
    glEnd();
}

void setText() {
    glColor3f(0.0, 0.0, 0.0);
    renderText("PID", x_left + g_width + 1, (2*sum_bottom + g_height)/2);
    renderText("P", x_left + g_width + 1, (2*kp_bottom + g_height)/2);
    renderText("I", x_left + g_width + 1, (2*ki_bottom + g_height)/2);
    renderText("D", x_left + g_width + 1, (2*kd_bottom + g_height)/2);
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

void graphInit(int* argc, char** argv) {
    kp_data = newLinkedList();
    ki_data = newLinkedList();
    kd_data = newLinkedList();
    kp_r = (2*kp_bottom + g_height)/2;
    ki_r = (2*ki_bottom + g_height)/2;
    kd_r = (2*kd_bottom + g_height)/2;
    sum_r = (2*sum_bottom + g_height)/2;
    boat_position = 0;

    openGLinit(argc, argv);
}
