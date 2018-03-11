#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "phidget22.h"
#ifndef _WIN32
#include <unistd.h>
#else
#include <Windows.h>
#endif

static void ssleep(int);

static void onAttachHandler(PhidgetHandle phid, void *ctx) {
	PhidgetReturnCode res;
	int hubPort;
	int channel;
	int serial;

	res = Phidget_getDeviceSerialNumber(phid, &serial);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to get device serial number\n");
		return;
	}

	res = Phidget_getChannel(phid, &channel);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to get channel number\n");
		return;
	}

	res = Phidget_getHubPort(phid, &hubPort);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to get hub port\n");
		hubPort = -1;
	}

	if (hubPort == -1)
		printf("channel %d on device %d attached\n", channel, serial);
	else
		printf("channel %d on device %d hub port %d attached\n", channel, serial, hubPort);
}

static void onDetachHandler(PhidgetHandle phid, void *ctx) {
	PhidgetReturnCode res;
	int hubPort;
	int channel;
	int serial;

	res = Phidget_getDeviceSerialNumber(phid, &serial);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to get device serial number\n");
		return;
	}

	res = Phidget_getChannel(phid, &channel);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to get channel number\n");
		return;
	}

	res = Phidget_getHubPort(phid, &hubPort);
	if (res != EPHIDGET_OK)
		hubPort = -1;

	if (hubPort != -1)
		printf("channel %d on device %d detached\n", channel, serial);
	else
		printf("channel %d on device %d hub port %d detached\n", channel, hubPort, serial);
}

static void errorHandler(PhidgetHandle phid, void *ctx, Phidget_ErrorEventCode errorCode, const char *errorString) {

	fprintf(stderr, "Error: %s (%d)\n", errorString, errorCode);
}

static void onPositionChangeHandler(PhidgetRCServoHandle ch, void *ctx, double position) {

	printf("Position Changed: %.3g\n", position);
}

static void onVelocityChangeHandler(PhidgetRCServoHandle ch, void *ctx, double velocity) {

	printf("Velocity Changed: %.3g\n", velocity);
}

static void onTargetPositionReachedHandler(PhidgetRCServoHandle ch, void *ctx, double position) {

	printf("Target Position Reached: %.3g\n", position);
}

/*
* Creates and initializes the channel.
*/
static PhidgetReturnCode initChannel(PhidgetHandle ch, int channel) {
	PhidgetReturnCode res;

	res = Phidget_setOnAttachHandler(ch, onAttachHandler, NULL);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to assign on attach handler\n");
		return (res);
	}

	res = Phidget_setOnDetachHandler(ch, onDetachHandler, NULL);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to assign on detach handler\n");
		return (res);
	}

	res = Phidget_setOnErrorHandler(ch, errorHandler, NULL);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to assign on error handler\n");
		return (res);
	}

    Phidget_setChannel(ch, channel);

	return (EPHIDGET_OK);
}

void printError(bool isInitialized, PhidgetReturnCode return_code, PhidgetHandle ch) {
    if (return_code != EPHIDGET_OK) {
        if (return_code == EPHIDGET_TIMEOUT) {
            printf("Timed out");
        }
        const char *error_desc;
        Phidget_getErrorDescription(return_code, &error_desc);
        fprintf(stderr, "%s\n", error_desc);

        if (isInitialized) {
            Phidget_close((PhidgetHandle) ch);
            PhidgetRCServo_delete(&ch);
        }
        exit(return_code);
    }
}

//void servoInit(PhidgetRCServoHandle* servohandle) {
PhidgetRCServoHandle* servoInit() {
	PhidgetRCServoHandle ch;
	PhidgetReturnCode res;
	const char *errs;
	res = PhidgetRCServo_create(&ch);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to create rc servo channel\n");
		exit(1);
	}

	res = initChannel((PhidgetHandle)ch, 0);
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

    int pos = 90;
	printf("Setting Target Position to 90 for %d seconds...\n", pos);
	PhidgetRCServo_setTargetPosition(*ch, pos);
	ssleep(5);
}

//====================VoltageInput====================
static void onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
	printf("Voltage Changed: %.4f\n", voltage);
}
//
PhidgetVoltageInputHandle* voltageInit() {
	PhidgetVoltageInputHandle ch;
	PhidgetReturnCode res;
	const char *errs;
	res = PhidgetVoltageInput_create(&ch);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to create voltage input channel\n");
		exit(1);
	}

	res = initChannel((PhidgetHandle)ch, 0);
	if (res != EPHIDGET_OK) {
		Phidget_getErrorDescription(res, &errs);
		fprintf(stderr, "failed to initialize channel:%s\n", errs);
		exit(1);
	}

	res = PhidgetVoltageInput_setOnVoltageChangeHandler(ch, onVoltageChangeHandler, NULL);
	if (res != EPHIDGET_OK) {
		Phidget_getErrorDescription(res, &errs);
		fprintf(stderr, "failed to set voltage change handler: %s\n", errs);
	    Phidget_close((PhidgetHandle)ch);
	    PhidgetVoltageInput_delete(&ch);
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
	    PhidgetVoltageInput_delete(&ch);
	    exit(res);
	}

	printf("Gathering data...\n");
    return &ch;
}
//====================VoltageInput====================


int main(int argc, char **argv) {
	PhidgetLog_enable(PHIDGET_LOG_INFO, NULL);
    PhidgetVoltageInputHandle vch = *voltageInit();
    PhidgetRCServoHandle ch = *servoInit();
    startServo(&ch);
}

static void ssleep(int tm) {
#ifdef _WIN32
	Sleep(tm * 1000);
#else
	sleep(tm);
#endif
}
