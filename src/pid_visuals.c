#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include "log_io.h"
#include "linked_list.h"
#include "pid_visuals.h"
#include "gl_framework.h"
#include "log_io.h"

FILE* pid_log;
double boatline = 10.7;
double boatline_thickness = 0.4;
double sum_bottom = 7.6;
double up_bottom = 5.1;
double ui_bottom = 2.6;
double ud_bottom = 0.1;
double g_width = 15.0;
double g_height = 2.4;
double x_left = 0.1;
double x_right;
double value_offset = -0.1;
char* sum_value[];
char* up_value[];
char* ui_value[];
char* ud_value[];
double up_r;
double ui_r;
double ud_r;
double sum_max;
double up_max;
double ui_max;
double ud_max;
double sum_r;
double boat_position;
char* boat_position_text[];
LinkedList* up_data;
LinkedList* ui_data;
LinkedList* ud_data;

double fRand(double f_min, double f_max) {
    double f = (double) rand() / RAND_MAX;
    return f*(f_max - f_min);
}

double scaleValue(double scale_min, double scale_max, double value_max, double value) {
    double percentage = value / value_max;
    return percentage*(scale_max - scale_min);
}

char* doubleToCharArray(double d, char* res) {
    snprintf(res, sizeof(res), "%4.7f", d);
    return res;
}

void drawLine(double x1, double y1, double x2, double y2, double axis) {
    // Clamp value to bounding box
    double lowest = axis - 0.5*g_height;
    double highest = axis + 0.5*g_height;

    double old_point = axis + y1;
    if (old_point < lowest)
        glVertex2f(x1, lowest);
    else if (old_point > highest)
        glVertex2f(x1, lowest);
    else
        glVertex2f(x1, old_point);

    double new_point = axis + y2;
    if (new_point < lowest)
        glVertex2f(x2, lowest);
    else if (new_point > highest)
        glVertex2f(x2, lowest);
    else
        glVertex2f(x2, new_point);
}

double step = 0.0001;
void renderLLData() {
    Node* current[] = {up_data->root, ui_data->root, ud_data->root};
    if (current[0] == NULL || current[0]->next == NULL)
        return;

    double counter = 0.0;
    double last_x; double current_x;
    glBegin(GL_LINES);
    glLineWidth(1.0);
    while (current[0]->next != NULL && current[1]->next != NULL && current[2]->next != NULL) {
        last_x = x_left + counter-step + 0.1;
        current_x = x_left + counter + 0.1;

        // pid graph
        glColor3ub(71, 144, 48);
        double pid_sum = current[0]->data + current[1]->data + current[2]->data;
        double next_pid_sum = current[0]->next->data + current[1]->next->data + current[2]->next->data;
        //glVertex2f(x_left + 0.1 + counter-step, sum_bottom + pid_sum/3.0f);
        //glVertex2f(x_left + 0.1 + counter, sum_bottom + next_pid_sum/3.0f);
        drawLine(last_x, scaleValue(0, 0.5*g_height, sum_max, pid_sum),
                current_x, scaleValue(0, 0.5*g_height, sum_max, next_pid_sum),
                sum_r);

        // up graph
        glColor3ub(71, 144, 48);
        drawLine(last_x, scaleValue(0, 0.5*g_height, up_max, current[0]->data),
                current_x, scaleValue(0, 0.5*g_height, up_max, current[0]->next->data),
                up_r);
        current[0] = current[0]->next;

        // ui graph
        glColor3ub(37, 87, 107);
        drawLine(last_x, scaleValue(0, 0.5*g_height, ui_max, current[1]->data),
                current_x, scaleValue(0, 0.5*g_height, ui_max, current[1]->next->data),
                ui_r);
        current[1] = current[1]->next;

        // ud graph
        glColor3ub(162, 54, 69);
        drawLine(last_x, scaleValue(0, 0.5*g_height, ud_max, current[2]->data),
                current_x, scaleValue(0, 0.5*g_height, ud_max, current[2]->next->data),
                ud_r);
        current[2] = current[2]->next;

        if (counter > g_width - 2*step) {
            leftLLPop(up_data);
            leftLLPop(ui_data);
            leftLLPop(ud_data);
        }
        else
            counter += step;
    }
    glEnd();
}

void rng() { // ONLY FOR TESTING
    addLLNode(up_data, fRand(up_bottom, up_bottom + g_height));
    addLLNode(ui_data, fRand(ui_bottom, ui_bottom + g_height));
    addLLNode(ud_data, fRand(ud_bottom, ud_bottom + g_height));
}


void addPIDNode(double up, double ui, double ud) {
    addLLNode(up_data, up);
    addLLNode(ui_data, ui);
    addLLNode(ud_data, ud);
}

void setBoatPosition(double position, double min_val, double max_val) {
    boat_position = x_left + scaleValue(0, g_width, max_val - min_val, position - min_val);
}

void drawBoat() {
    glColor3f(0, 0, 0);
    glRectf(boat_position - 0.05, boatline - 0.5, boat_position + 0.05, boatline + boatline_thickness + 0.4);
}

void drawLayout() {
    // Boat position rectangle
    glColor3ub(7, 102, 120);
    glRectf(x_left, boatline, x_right, boatline + boatline_thickness);

    // Rectangles for the four graphs
    glColor3ub(251, 241, 199);
    glRectf(x_left, sum_bottom, x_right, sum_bottom + g_height);
    glRectf(x_left, up_bottom, x_right, up_bottom + g_height);
    glRectf(x_left, ui_bottom, x_right, ui_bottom + g_height);
    glRectf(x_left, ud_bottom, x_right, ud_bottom + g_height);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    // Axis line for PID output
    glVertex3f(x_left, sum_r, 0);
    glVertex3f(x_right, sum_r, 0);

    // Axis line for the proportional term
    glVertex3f(x_left, up_r, 0);
    glVertex3f(x_right, up_r, 0);

    // Axis line for the integral term
    glVertex3f(x_left, ui_r, 0);
    glVertex3f(x_right, ui_r, 0);

    // Axis line for the derivative term
    glVertex3f(x_left, ud_r, 0);
    glVertex3f(x_right, ud_r, 0);
    glEnd();
}

void setText() {
    glColor3f(0.0, 1.0, 0.0);
    renderText("ref", x_right + 0.5, boatline + 0.5);
    glColor3f(0.0, 0.0, 1.0);
    renderText("pos", x_right + 0.5, boatline);
    glColor3f(1.0, 0.0, 0.0);
    renderText("err", x_right + 0.5, boatline - 0.5);
    glColor3f(0.0, 0.0, 0.0);
    renderText("PID", x_right + 0.5, sum_r + value_offset);
    renderText("P", x_right + 0.5, up_r + value_offset);
    renderText("I", x_right + 0.5, ui_r + value_offset);
    renderText("D", x_right + 0.5, ud_r + value_offset);
    if (up_data->tail != NULL && ui_data->tail != NULL && ud_data->tail != NULL) {
        //TODO: boat position, reference and error
        doubleToCharArray(boat_position, boat_position_text);
        renderText(boat_position_text, x_right + 1.5, boatline);

        doubleToCharArray(up_data->tail->data +
                ui_data->tail->data +
                ud_data->tail->data,
                sum_value);
        renderText(sum_value, x_right + 1.5, sum_r + value_offset);

        doubleToCharArray(up_data->tail->data, up_value);
        renderText(up_value, x_right + 1.5, up_r + value_offset);

        doubleToCharArray(ui_data->tail->data, ui_value);
        renderText(ui_value, x_right + 1.5, ui_r + value_offset);

        doubleToCharArray(ud_data->tail->data, ud_value);
        renderText(ud_value, x_right + 1.5, ud_r + value_offset);
    }
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

void graphInit(int* argc, char** argv, char* log_path) {
//    FILE* f = openFile("logs/log1.txt", "r");
//    char line[128];
//    while(nextLine(f, line)) {
//        printf("%s", line);
//        sleep(1);
//    }
//    printf("done\n");
//    closeFile(f);

    double p_scale;
    double i_scale;
    double d_scale;
    up_data = newLinkedList();
    ui_data = newLinkedList();
    ud_data = newLinkedList();
    up_r = up_bottom + 0.5*g_height;
    ui_r = ui_bottom + 0.5*g_height;
    ud_r = ud_bottom + 0.5*g_height;
    sum_r = sum_bottom + 0.5*g_height;
    x_right = x_left + g_width;
    boat_position = 0;

    if (log_path) {
        FILE* f = openFile(log_path, "r");
        char line[128];
        while (nextLine(f, line)) {
            // run log
        }
    }
    else {
        p_scale = 2.0;
        i_scale = 100000.0;
        d_scale = 0.05;
        up_max = atof(argv[1])*p_scale;
        ui_max = atof(argv[2])*i_scale;
        ud_max = atof(argv[3])*d_scale;
        sum_max = up_max;
        // log this
        printf("p_scale %f, i_scale %f, d_scale %f\n", p_scale, i_scale, d_scale);
    }

    openGLinit(argc, argv);
}

