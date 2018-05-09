#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "phidget22.h"
#include <unistd.h>
#include "phidget_tools.h"
#include "servo.h"
#include "voltage_input.h"
#include "pid_visuals.h"
#include "controllers.h"
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

double scaleOutput(double value, struct MotorSettings settings) {
    if (value > 100) {
        return settings.motor_max;
    } else if (value < 0) {
        return settings.motor_min;
    } else {
        return settings.motor_min + value*(settings.motor_max - settings.motor_min)/100;
    }
}

struct MotorSettings motor_settings;

void setMotorOutput(struct DPContext ctx, double output) {
    double scaled_output = scaleOutput(-output, motor_settings);
    //printf("OUTPUT: %f\n", scaled_output);
    PhidgetRCServo_setTargetPosition(ctx.ch, scaled_output);
}

double reference;
void motorController(double Kp, double Ki, double Kd, double dt, struct DPContext ctx) {
    double boat_position;
    double output;
    PhidgetVoltageInput_getSensorValue(&ctx.vch, &boat_position);

    while (true) {
        //boat_position = sampler(boat_position, 0.85, 20, dt, &ctx.vch);
        PhidgetVoltageInput_getSensorValue(ctx.vch, &boat_position);
        setBoatPosition(boat_position, 3.55, 5.0);
        //printf("position: %f, error: %f (%f)\n", boat_position, reference - boat_position, reference);
        //printf("ref %f pos %f error %f\n", reference, boat_position, reference - boat_position);
        output = calculatePIDOutput(Kp, Ki, Kd, reference - boat_position, dt);
        setMotorOutput(ctx, output);
    }
}

void test_motor(PhidgetRCServoHandle* ch) {
    double test_input;
    while (true) {
        scanf("%lf", &test_input);
        PhidgetRCServo_setTargetPosition(*ch, test_input);
    }
}

int main(int argc, char** argv) {
//    if (argc !=  6) {
//        printf("Error: expecting 6 args, got %d\n", argc);
//        printf("Hint: ./dynpos Kp Ti Td reference dt\n");
//        printf("Exiting..\n");
//        exit(1);
//    }
//    double Kp = atof(argv[1]);
//    double Ki = atof(argv[2]);
//    double Kd = atof(argv[3]);
//    //double reference = atof(argv[4]);
//    reference = atof(argv[4]);
//    double dt = atoi(argv[5]);
//    printf("Kp: %lf, Ki: %lf, Kd: %lf, reference: %lf, dt: %d\n", Kp, Ki, Kd, reference, dt);
//
//    motor_settings.motor_min = 72.0;
//    motor_settings.motor_max = 152.0;

    graphInit(&argc, argv, NULL);

//    PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
//    struct DPContext context;
//    context.vch = *voltageInit(0);
//    context.ch = *servoInit(0);
//    startServo(&context.ch);
//    //test_motor(&context.ch);
//    //PID_controller(Kp, Ki, Kd, dt, reference, context);
//    motorController(Kp, Ki, Kd, dt, context);



//================= TESTING====================
    double counter = 0;
    double step = 0.01;
    while (true) {
        rng();
        usleep(100000);

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

