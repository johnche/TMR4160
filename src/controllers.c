#include "pid_visuals.h"
#include "controllers.h"

double PController(double Kp, double error) {
    return Kp * error;
}

double IController(double Ki, double error, double dt, double i_prev, double threshold) {
    double retval = i_prev + Ki*dt*error;
    if (retval > threshold)
        return threshold;
    else if (retval < -threshold)
        return -threshold;
    return retval;
}

double DController(double Kd, double error, double dt, double error_prev) {
    return Kd*(error - error_prev)/dt;
}

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

//double PID_withPrev(double error0, double error1, double error2,
//        double Kp, double Ti, double Td, double dt, double uprev) {
//    double up = error0 - error1;
//    double ui = (error0 * dt)/Ti;
//    double ud = (Td/dt)*(error0 - 2*error1 + error2);
//    return Kp*(up + ui + ud) + uprev;
//}


