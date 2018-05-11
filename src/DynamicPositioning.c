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

/*
 * Main file, which initiates the different systems including
 * servo, voltageinput, PID controller and the GUI framework.
 */

/*
 * Holds the handler datatypes which will be used to communicate
 * with the phidget framework
 */
struct DPContext {
    PhidgetVoltageInputHandle vch;
    PhidgetRCServoHandle ch;
};

/*
 * This is later used to clamp PID output between tested max
 * and min values which the actuator responds to.
 */
struct MotorSettings {
    double motor_max;
    double motor_min;
};
struct MotorSettings motor_settings;

/*
 * [reference] holds the reference value which the controller
 * base the positional error on.
 *
 * [b_min] and [b_max] holds the max and min values of what
 * position the boat can be in (so that a sensible position
 * graph can be drawn.
 *
 * [r_step] is the size which the reference changes for
 * each time the left or right key is pressed.
 */
double reference;
double b_min;
double b_max;
double r_step = 0.1;

/*
 * GLUT keyboard callbacks
 */
void keyPressed(unsigned char key, int x, int y) {
    printf("key %d\n", key);
    switch (key) {
        case 32: // space is pressed
            togglePositionGraph();
            break;
        case 27: // esc is pressed
            exitPIDVisuals();
            printf("Exiting..");
            exit(0);
            break;
    }
}

void specialKeyPressed(unsigned char key, int x, int y) {
    printf("specialkey %d\n", key);
    switch (key) {
        case 100: // left arrow is pressed
            reference -= r_step;
            if (reference < b_min)
                reference = b_min;
            updateReference(reference);
            break;
        case 102: // right arrow is pressed
            reference += r_step;
            if (reference > b_max)
                reference = b_max;
            updateReference(reference);
            break;
    }
}

/*
 * The boat motor only responds on certain blobs of values. motor_max and
 * motor_min are empirically tested max and min values for outputing positive
 * torque. If the PID output (value parameter in the function signature) is
 * bigger or smaller than the recorded extremals, the output value is clamped
 * between before returned.
 */
double scaleOutput(double value, struct MotorSettings settings) {
    if (value > 100) {
        return settings.motor_max;
    } else if (value < 0) {
        return settings.motor_min;
    } else {
        return settings.motor_min + value*(settings.motor_max - settings.motor_min)/100;
    }
}

/*
 * Once the PID output has been scaled between sensible motor values,
 * it is sent to the actuator through PhidgetRCServo_setTargetPosition.
 */
void setMotorOutput(struct DPContext ctx, double pid_output) {
    double scaled_output = scaleOutput(-pid_output, motor_settings);
    PhidgetRCServo_setTargetPosition(ctx.ch, scaled_output);
}

/*
 * Controller main loop.
 * Checks the boat position, calculates error from this and fetches
 * a PID response from calculatePIDOutput. Sends the PID response for
 * further processing (setMotorOutput) before it reaches the servo.
 * Sleeps for a given amount of time [dt] milliseconds for
 * samplingrate control.
 */
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

/*
 * Not fully integrated feature.
 * This allows userinput straight to the servo.
 * The intention for this function is to map the motor response
 * boundaries, which the rest of the program is built on.
 */
void test_motor(PhidgetRCServoHandle* ch) {
    double test_input;
    while (true) {
        scanf("%lf", &test_input);
        PhidgetRCServo_setTargetPosition(*ch, test_input);
    }
}

/*
 * Main function.
 * Grabs program arguments and initializes central parameters of
 * the controllers. Proceeds to run the initializers for phidget
 * devices and GUI before running the main controller loop
 * (motorController).
 * When initializing the GUI (graphInit), it also passes callbacks/
 * function pointers to keypress handlers.
 */
int main(int argc, char** argv) {
    if (argc == 2)
        graphInit(&argc, argv, 5.0, 3.55, keyPressed, specialKeyPressed, argv[1]);
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

        b_max = 5.0;
        b_min = 3.55;
        motor_settings.motor_min = 72.0;
        motor_settings.motor_max = 142.0;


        PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
        struct DPContext context;
        context.vch = *voltageInit(0);
        context.ch = *servoInit(0);
        startServo(&context.ch);
//        test_motor(&context.ch);
        graphInit(&argc, argv, b_max, b_min, keyPressed, specialKeyPressed, NULL);
        motorController(Kp, Ki, Kd, dt, context);
    }
}

