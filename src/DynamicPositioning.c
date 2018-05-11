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

double reference;
double b_min = 3.55;
double b_max = 5.0;
double r_step = 0.1;

void keyPressed(unsigned char key, int x, int y) {
    printf("key %d\n", key);
    switch (key) {
        case 32:
            togglePositionGraph();
            break;
        case 27:
            exitPIDVisuals();
            printf("Exiting..");
            exit(0);
            break;
    }
}

void specialKeyPressed(unsigned char key, int x, int y) {
    printf("specialkey %d\n", key);
    switch (key) {
        case 100:
            reference -= r_step;
            if (reference < b_min)
                reference = b_min;
            updateReference(reference);
            break;
        case 102:
            reference += r_step;
            if (reference > b_max)
                reference = b_max;
            updateReference(reference);
            break;
    }
}

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
    PhidgetRCServo_setTargetPosition(ctx.ch, scaled_output);
}

void motorController(double Kp, double Ki, double Kd, double dt, struct DPContext ctx) {
    double boat_position;
    double output;
    PhidgetVoltageInput_getSensorValue(&ctx.vch, &boat_position);

    while (true) {
        PhidgetVoltageInput_getSensorValue(ctx.vch, &boat_position);
        updateBoatPosition(boat_position);
        output = calculatePIDOutput(Kp, Ki, Kd, reference - boat_position, dt);
        setMotorOutput(ctx, output);
        usleep(1000 * dt);
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
    if (argc == 2)
        graphInit(&argc, argv, b_max, b_min, keyPressed, specialKeyPressed, argv[1]);
    else if (argc !=  6) {
        printf("Error. Please input arguments\n");
        printf("Standard mode: ./dynpos Kp Ti Td reference dt\n");
        printf("Log mode: ./dynpos path_to_log\n");
        printf("Exiting..\n");
        exit(1);
    }
    else {
        double Kp = atof(argv[1]);
        double Ki = atof(argv[2]);
        double Kd = atof(argv[3]);
        reference = atof(argv[4]);
        int dt = atoi(argv[5]);
        printf("Kp: %lf, Ki: %lf, Kd: %lf, reference: %lf, dt: %d\n", Kp, Ki, Kd, reference, dt);

        motor_settings.motor_min = 72.0;
        motor_settings.motor_max = 142.0;


        PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
        struct DPContext context;
        context.vch = *voltageInit(0);
        context.ch = *servoInit(0);
        startServo(&context.ch);
    //    //test_motor(&context.ch);
        graphInit(&argc, argv, b_max, b_min, keyPressed, specialKeyPressed, NULL);
        motorController(Kp, Ki, Kd, dt, context);
    }


//================= TESTING====================
//    graphInit(&argc, argv, 5.0, 0, keyPressed, specialKeyPressed, NULL);
//    b_min = 0;
//    double counter = 0;
//    double step = 0.01;
//    while (true) {
//        rng();
//        usleep(10000);
//
//        updateBoatPosition(counter);
//        counter += step;
//        if (counter > 5) {
//            counter = 5;
//            step *= -1;
//        } else if (counter < 0) {
//            counter = 0;
//            step *= -1;
//        }
//    }


}

