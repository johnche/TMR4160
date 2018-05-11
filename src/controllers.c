#include "pid_visuals.h"
#include "controllers.h"

/*
 * This file contains the logic of the controllers.
 */

/*
 * Scales output proportional to the error and the gain
 */
double PController(double Kp, double error) {
    return Kp * error;
}

/*
 * Returns an output which is accumulated error over time.
 * Clamps the value to be within a threshold.
 */
double IController(double Ki, double error, double dt, double i_prev, double threshold) {
    double retval = i_prev + Ki*dt*error;
    if (retval > threshold)
        return threshold;
    else if (retval < -threshold)
        return -threshold;
    return retval;
}

/*
 * Returns an output based on the change of the error.
 */
double DController(double Kd, double error, double dt, double error_prev) {
    return Kd*(error - error_prev)/dt;
}

/*
 * Returns the sum of the outputs from the controllers above. This becomes our
 * PID controller output. It reports in addition the output from each term to
 * the GUI framework through addPIDNode, which attaches the data as the last node
 * in a linked list.
 */
double error_prev = 0;
double i_prev = 0;
double calculatePIDOutput(double Kp, double Ki, double Kd, double error, double dt) {
    double up = PController(Kp, error);
    double ui = IController(Ki, error, dt, i_prev, 500);
    double ud = DController(Kd, error, dt, error_prev);

    addPIDNode(up, ui, ud);
    error_prev = error;
    i_prev = ui;
    return up + ui + ud;
}

