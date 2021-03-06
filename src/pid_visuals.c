#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include "log_io.h"
#include "linked_list.h"
#include "pid_visuals.h"
#include "gl_framework.h"
#include "log_io.h"

/* Main implementation of the GUI.
 * Lots of evil global variables.
 * Most to set layout anchors, others just for buffer allocation.
 * Main logic can be categorized in layout and actors. The layout
 * describes most static things, while the actors are dynamically
 * changed during the session.
 */

FILE* pid_log;

// Width resolution of the graphs
double step = 0.005;

// Layout anchors
double boatline = 10.6;
double boatline_thickness = 0.4;
double boat_bottom = 10.0;
double boat_height = 1.5;
double sum_bottom = 7.5;
double up_bottom = 5.0;
double ui_bottom = 2.5;
double ud_bottom = 0.0;
double g_width = 15.0;
double g_height = 2.4;
double x_left = 0.1;
double x_right;
double value_offset = -0.1;
double up_r, ui_r, ud_r, sum_r;
double sum_max, up_max, ui_max, ud_max;
double boat_position, boat_max, boat_min;
double reference;
double boat_rect_left;
double ref_rect_left;

// Value-to-GUI buffers
char* sum_value[];
char* up_value[];
char *ui_value[];
char *ud_value[];
char* ref_position_text[];
char* boat_position_text[];
char* err_position_text[];

// Toggle variable for displaying position graph
bool show_pos_graph = false;

// Switch between logging or reading log
bool log_mode;

// Linked list containing data to be drawn
LinkedList* pos_data;
LinkedList* up_data;
LinkedList* ui_data;
LinkedList* ud_data;


double scaleValue(double scale_min, double scale_max, double value_max, double value) {
    double percentage = value / value_max;
    return percentage*(scale_max - scale_min);
}

char* doubleToCharArray(double d, char* res) {
    snprintf(res, sizeof(res), "%4.7f", d);
    return res;
}

void updateBoatPosition(double position) {
    boat_position = position; // TODO: deprecate this
    addLLNode(pos_data, position);
}

void updateReference(double new_reference) {
    reference = new_reference;
}

void togglePositionGraph() {
    show_pos_graph = !show_pos_graph;
}

void exitPIDVisuals() {
    closeFile(pid_log);
}

/*
 * Adds a new node to each respective list when new PID values are registered.
 * All points in these lists will be drawn later.
 * If the system is replaying a log, skip writing the values to file.
 */
void addPIDNode(double up, double ui, double ud) {
    addLLNode(up_data, up);
    addLLNode(ui_data, ui);
    addLLNode(ud_data, ud);
    if (!log_mode && pos_data->tail != NULL)
        writePIDValues(pid_log, up, ui, ud, pos_data->tail->data, reference);
}

/*
 * Draws a line, but clamps the y values in a radius half of g_height from axis.
 */
void drawClampedLine(double x1, double y1, double x2, double y2, double axis) {
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

/*
 * Draws lines between each node from the nodes of the linked lists.
 * Removes the oldest nodes if the number of nodes in linked lists are
 * bigger than the number of vertical sections each graph box has.
 */
void renderLLData() {
    Node* current[] = {up_data->root, ui_data->root, ud_data->root, pos_data->root};

    double counter = 0.0;
    double last_x; double current_x;
    glBegin(GL_LINES);
    glLineWidth(5.0);
    while (current[0]->next != NULL &&
            current[1]->next != NULL &&
            current[2]->next != NULL &&
            current[3]->next != NULL) {
        last_x = x_left + counter-step;
        current_x = x_left + counter;

        // position graph
        glColor3ub(71, 144, 48);
        if (show_pos_graph) {
            drawClampedLine(last_x,
                    scaleValue(0, boat_height,
                        boat_max - boat_min,
                        current[3]->data - boat_min) - 0.5*boat_height,
                    current_x,
                    scaleValue(0,
                        boat_height,
                        boat_max - boat_min,
                        current[3]->next->data - boat_min) - 0.5*boat_height,
                    boat_bottom + 0.5*boat_height);
        }
        current[3] = current[3]->next;

        // pid graph
        glColor3ub(69, 133, 136);
        double pid_sum = current[0]->data + current[1]->data + current[2]->data;
        double next_pid_sum = current[0]->next->data +
            current[1]->next->data +
            current[2]->next->data;
        drawClampedLine(last_x, scaleValue(0, 0.5*g_height, sum_max, pid_sum),
                current_x, scaleValue(0, 0.5*g_height, sum_max, next_pid_sum),
                sum_r);

        // up graph
        glColor3ub(142, 192, 124);
        drawClampedLine(last_x, scaleValue(0, 0.5*g_height, up_max, current[0]->data),
                current_x, scaleValue(0, 0.5*g_height, up_max, current[0]->next->data),
                up_r);
        current[0] = current[0]->next;

        // ui graph
        glColor3ub(250, 189, 47);
        drawClampedLine(last_x, scaleValue(0, 0.5*g_height, ui_max, current[1]->data),
                current_x, scaleValue(0, 0.5*g_height, ui_max, current[1]->next->data),
                ui_r);
        current[1] = current[1]->next;

        // ud graph
        glColor3ub(251, 73, 52);
        drawClampedLine(last_x, scaleValue(0, 0.5*g_height, ud_max, current[2]->data),
                current_x, scaleValue(0, 0.5*g_height, ud_max, current[2]->next->data),
                ud_r);
        current[2] = current[2]->next;

        if (counter > g_width - 2*step) {
            leftLLPop(pos_data);
            leftLLPop(up_data);
            leftLLPop(ui_data);
            leftLLPop(ud_data);
        }
        else
            counter += step;
    }
    glEnd();
}

/*
 * Draws the position of the boat when not showing xy-position.
 */
void drawBoat() {
    if (!show_pos_graph) {
        glColor3f(0, 0, 0);
        boat_rect_left = x_left +
            scaleValue(0, g_width, boat_max - boat_min, boat_position - boat_min);

        glRectf(boat_rect_left - 0.05,
                boatline - 0.5,
                boat_rect_left + 0.05,
                boatline + boatline_thickness + 0.4);
    }
}

/*
 * Draws the reference. Based on position toggle variable, it will draw
 * the reference either as a box in a one-dimentional visualization or
 * a line in the xy-position graph.
 */
double ref_line;
void drawReference() {
    glColor4ub(200, 0, 0, 255);
    if (show_pos_graph) {
        ref_line = boat_bottom +
            scaleValue(0, boat_height, boat_max - boat_min, reference - boat_min);
        glBegin(GL_LINES);
        glVertex2f(x_left, ref_line);
        glVertex2f(x_right, ref_line);
        glEnd();
    }
    else {
        ref_rect_left = x_left +
            scaleValue(0, g_width, boat_max - boat_min, reference - boat_min);
        glRectf(ref_rect_left - 0.05,
                boatline - 0.5,
                ref_rect_left + 0.05,
                boatline + boatline_thickness + 0.4);
    }
}

/*
 * Draws static text and parameter values during the session.
 */
void setValuesText() {
    if (up_data->tail != NULL && ui_data->tail != NULL && ud_data->tail != NULL) {
        //TODO: boat position, reference and error
        doubleToCharArray(reference, ref_position_text);
        renderText(ref_position_text, x_right + 1.5, boatline + 0.5);
        doubleToCharArray(boat_position, boat_position_text);
        renderText(boat_position_text, x_right + 1.5, boatline);
        doubleToCharArray(reference - boat_position, err_position_text);
        renderText(err_position_text, x_right + 1.5, boatline - 0.5);

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

/*
 * Draws the graph background rectangles.
 * The position of these rectangles are anchored by the anchor variables
 * on top of this file.
 */
void drawLayout() {
    // Boat position rectangle
    if (show_pos_graph) {
        glColor3ub(235, 235, 235);
        glRectf(x_left, boat_bottom, x_right, boat_bottom + boat_height);
    }
    else {
        glColor3ub(7, 102, 120);
        glRectf(x_left, boatline, x_right, boatline + boatline_thickness);
    }

    // Rectangles for the four graphs
    glColor3ub(235, 235, 235);
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

    // Set static text
    glColor3f(0.0, 1.0, 0.0);
    renderText("ref", x_right + 0.5, boatline + 0.5);
    glColor3f(0.0, 0.0, 1.0);
    renderText("pos", x_right + 0.5, boatline);
    glColor3f(1.0, 0.0, 0.0);
    renderText("err", x_right + 0.5, boatline - 0.5);
    glColor3ub(146, 131, 116);
    renderText("PID", x_right + 0.5, sum_r + value_offset);
    renderText("P", x_right + 0.5, up_r + value_offset);
    renderText("I", x_right + 0.5, ui_r + value_offset);
    renderText("D", x_right + 0.5, ud_r + value_offset);

}

/*
 * Does a collection of calls to function which draws dynamic data.
 */
void drawActors() {
    setValuesText();
    renderLLData();
    drawBoat();
    drawReference();
}

/*
 * Initializes variables and branches into either log mode or live mode
 * depending on the log_path argument. Live mode will record the headers
 * before continuing to run the rest of the program, log mode will unpack
 * the values from the file specified by log_path,
 */
void graphInit(int* argc,
        char** argv,
        double b_max,
        double b_min,
        void (*keyPressed)(unsigned char, int, int),
        void (*specialKeyPressed)(unsigned char, int, int),
        char* log_path) {

    double p_scale;
    double i_scale;
    double d_scale;
    pos_data = newLinkedList();
    up_data = newLinkedList();
    ui_data = newLinkedList();
    ud_data = newLinkedList();
    boat_max = b_max;
    boat_min = b_min;
    up_r = up_bottom + 0.5*g_height;
    ui_r = ui_bottom + 0.5*g_height;
    ud_r = ud_bottom + 0.5*g_height;
    sum_r = sum_bottom + 0.5*g_height;
    x_right = x_left + g_width;
    boat_position = 0;


    if (log_path) {
        log_mode = true;
        double kp, ki, kd, up, ui, ud, pos;
        int sampled_rate;
        char line[128];
        pid_log = openFile(log_path, "r");
        nextLine(pid_log, line, sizeof(line));
        sscanf(line, "%lf %lf %lf %d %lf %lf %lf",
                &kp, &ki, &kd, &sampled_rate, &p_scale, &i_scale, &d_scale);
        up_max = kp*p_scale;
        ui_max = ki*i_scale;
        ud_max = kd*d_scale;
        sum_max = up_max;

        openGLinit(argc, argv, keyPressed, specialKeyPressed);
        while (nextLine(pid_log, line, sizeof(line))) {
            sscanf(line, "%lf %lf %lf %lf %lf", &up, &ui, &ud, &pos, &reference);
            updateBoatPosition(pos);
            addPIDNode(up, ui, ud);
            usleep(1000*sampled_rate);
        }
    }
    else {
        log_mode = false;
        pid_log = openFile(log_path, "w");
        p_scale = 2.0;
        i_scale = 100000.0;
        d_scale = 0.05;
        up_max = atof(argv[1])*p_scale;
        ui_max = atof(argv[2])*i_scale;
        ud_max = atof(argv[3])*d_scale;
        sum_max = up_max;
        reference = atof(argv[4]);
        writeHeaderValues(pid_log,
                atof(argv[1]), atof(argv[2]), atof(argv[3]), atoi(argv[5]),
                p_scale, i_scale, d_scale);
        openGLinit(argc, argv, keyPressed, specialKeyPressed);
    }
}

