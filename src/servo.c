#include <stdio.h>
#include "phidget22.h"
#include "phidget_tools.h"

/*
 * Logic containing initializers for servo communication
 */

static void onPositionChangeHandler(PhidgetRCServoHandle ch, void *ctx, double position) {
	//printf("Position Changed: %.3g\n", position);
}

static void onVelocityChangeHandler(PhidgetRCServoHandle ch, void *ctx, double velocity) {
	//printf("Velocity Changed: %.3g\n", velocity);
}

static void onTargetPositionReachedHandler(PhidgetRCServoHandle ch, void *ctx, double position) {
	//printf("Target Position Reached: %.3g\n", position);
}

/*
 * Returns a phidget handler which can be used other places to communicate
 * with the phidget devices.
 */
PhidgetRCServoHandle* servoInit(int channel) {
	PhidgetRCServoHandle ch;
	PhidgetReturnCode res;
	const char *errs;
	res = PhidgetRCServo_create(&ch);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to create rc servo channel\n");
		exit(1);
	}

	res = initChannel((PhidgetHandle)ch, channel);
	if (res != EPHIDGET_OK) {
		Phidget_getErrorDescription(res, &errs);
		fprintf(stderr, "failed to initialize channel:%s\n", errs);
		exit(1);
	}

	res = PhidgetRCServo_setOnPositionChangeHandler(ch, onPositionChangeHandler, NULL);
	if (res != EPHIDGET_OK) {
		Phidget_getErrorDescription(res, &errs);
		fprintf(stderr, "failed to set position change handler: %s\n", errs);
	    Phidget_close((PhidgetHandle)ch);
	    PhidgetRCServo_delete(&ch);
	    exit(res);
	}

	res = PhidgetRCServo_setOnVelocityChangeHandler(ch, onVelocityChangeHandler, NULL);
	if (res != EPHIDGET_OK) {
		Phidget_getErrorDescription(res, &errs);
		fprintf(stderr, "failed to set velocity change handler: %s\n", errs);
	    Phidget_close((PhidgetHandle)ch);
	    PhidgetRCServo_delete(&ch);
	    exit(res);
	}

	res = PhidgetRCServo_setOnTargetPositionReachedHandler(ch, onTargetPositionReachedHandler, NULL);
	if (res != EPHIDGET_OK) {
		Phidget_getErrorDescription(res, &errs);
		fprintf(stderr, "failed to set target position reached handler: %s\n", errs);
	    Phidget_close((PhidgetHandle)ch);
	    PhidgetRCServo_delete(&ch);
	    exit(res);
	}

	/*
	* Open the channel synchronously: waiting a maximum of 5 seconds.
	*/
	res = Phidget_openWaitForAttachment((PhidgetHandle)ch, 5000);
	if (res != EPHIDGET_OK) {
		if (res == EPHIDGET_TIMEOUT) {
			printf("Channel did not attach after 5 seconds: please check that the device is attached\n");
		} else {
			Phidget_getErrorDescription(res, &errs);
			fprintf(stderr, "failed to open channel:%s\n", errs);
		}
	    Phidget_close((PhidgetHandle)ch);
	    PhidgetRCServo_delete(&ch);
	    exit(res);
	}

    printf("ENDOF\n");

    return &ch;
}

/*
 * The phidget framework requires a target position before setEngaged can be called.
 */
void startServo(PhidgetRCServoHandle* ch) {
    PhidgetReturnCode res;
	printf("Setting target position to 90\n");
	res = PhidgetRCServo_setTargetPosition(*ch, 90);
	if (res != EPHIDGET_OK) {
		printf("failed to set target position\n");
	}

	printf("Setting engaged\n");
	res = PhidgetRCServo_setEngaged(*ch, 1);
	if (res != EPHIDGET_OK) {
		printf("failed to set engaged\n");
	}

}

