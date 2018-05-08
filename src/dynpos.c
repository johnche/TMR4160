#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "phidget22.h"
#include <unistd.h>
#include "phidget_tools.h"
#include "servo.h"
#include "voltage_input.h"
#include "glplot.h"
#ifndef _WIN32
#include <unistd.h>
#else
#include <Windows.h>
#endif

struct DPContext {
    PhidgetVoltageInputHandle vch;
    PhidgetRCServoHandle ch;
};

struct MotorSettings {
    double motor_max;
    double motor_min;
};

double PID_withPrev(double error0, double error1, double error2,
        double Kp, double Ti, double Td, double dt, double uprev) {
    double up = error0 - error1;
    double ui = (error0 * dt)/Ti;
    double ud = (Td/dt)*(error0 - 2*error1 + error2);
    return Kp*(up + ui + ud) + uprev;
}

double sampler(double start_value, double constant,
        int pid_frequency, int dt, PhidgetVoltageInputHandle* vch) {
    double new_value; double old_value; double res;
    double test1; double test2;
    for (int i = 0; i < pid_frequency; i++) {
        old_value = start_value;
        PhidgetVoltageInput_getSensorValue(*vch, &new_value);
        //new_value = old_value*constant + new_value*(1 - constant);
        //res = old_value*constant + new_value*(1 - constant);
        test1 = old_value*constant;
        test2 = new_value*(1-constant);
        res = test1 + test2;
        printf("test1 %lf test2 %lf\n", test1, test2);
        printf("polled %lf  smoothed(res) %lf\n", new_value, res);
        new_value = res;
        usleep(1000*dt);
    }

    return new_value;
}

double scale_motor_output(double value, struct MotorSettings settings) {
    if (value > 100) {
        return settings.motor_max;
    } else if (value < 0) {
        return settings.motor_min;
    } else {
        return settings.motor_min + value*(settings.motor_max - settings.motor_min)/100;
    }
}

double PID_controller(double Kp, double Ki, double Kd, double dt,
        double reference, struct DPContext context) {
    double i_prev;
    double error; double error_prev;
    double up; double ui; double ud; double output;

    double boat_position;
    PhidgetVoltageInput_getSensorValue(&context.vch, &boat_position);

    printf("boat position: %lf\n", boat_position);

    struct MotorSettings motor_settings;
    motor_settings.motor_min = 62.1;
    motor_settings.motor_max = 64.8;
    int sock = get_socket("0.0.0.0", "5002");
    char* message;

    //PhidgetRCServo_setTargetPosition(context.ch, 0);
    //sleep(5);
    //PhidgetRCServo_setTargetPosition(context.ch, 90);
    //sleep(5);
    //PhidgetRCServo_setTargetPosition(context.ch, 180);
    //sleep(5);
    while (true) {
        boat_position = sampler(boat_position, 0.85, 20, dt, &context.vch);
        //printf("PID %lf\n", boat_position);
        error = reference - boat_position;
        up = error;
        ui = i_prev + Ki*dt*error;
        ud = Kd*(error - error_prev)/dt;

        error_prev = error;
        i_prev = ui;

        //printf("%lf %lf %lf %lf\n", up, ui, ud, Kp);
        output = scale_motor_output(Kp*(up + ui + ud)*-1, motor_settings);
        printf("output: %lf, position: %lf\n", output, boat_position);
        asprintf(&message, "%lf %lf %lf %lf %lf\n",  up, ui, ud, Kp, output);
        writedata(sock, message, strlen(message));
        PhidgetRCServo_setTargetPosition(context.ch, output);
    }
    return 0.0;
}

void test_motor(PhidgetRCServoHandle* ch) {
    double test_input;
    while (true) {
        scanf("%lf", &test_input);
        PhidgetRCServo_setTargetPosition(*ch, test_input);
    }
}

int main(int argc, char** argv) {
 //   if (argc !=  6) {
 //       printf("Error: expecting 6 args, got %d\n", argc);
 //       printf("Hint: ./dynpos Kp Ti Td reference dt\n");
 //       printf("Exiting..\n");
 //       exit(1);
 //   }
 //   double Kp = atof(argv[1]);
 //   double Ki = atof(argv[2]);
 //   double Kd = atof(argv[3]);
 //   double reference = atof(argv[4]);
 //   double dt = atoi(argv[5]);
 //   printf("Kp: %lf, Ki: %lf, Kd: %lf, reference: %lf, dt: %d\n", Kp, Ki, Kd, reference, dt);

    openGLinit(&argc, argv);

//    PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
//    struct DPContext context;
//    context.vch = *voltageInit(0);
//    context.ch = *servoInit(0);
//    startServo(&context.ch);
//    //test_motor(&context.ch);
//    PID_controller(Kp, Ki, Kd, dt, reference, context);


    double counter = 0;
    double step = 0.01;
    while (true) {
        rng();
        usleep(10000);

        setBoatPosition(counter, 0.0, 5.0);
        counter += step;
        if (counter > 5) {
            counter = 5;
            step *= -1;
        } else if (counter < 0) {
            counter = 0;
            step *= -1;
        }
    }


}

