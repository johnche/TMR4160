#include <stdio.h>
#include "phidget22.h"
#include "phidget_tools.h"

/*
 * Initializer for voltage input.
 */

static void onVoltageChangeHandler(PhidgetVoltageInputHandle ch, void *ctx, double voltage) {
	//printf("Voltage Changed: %.4f\n", voltage);
}

/*
 * Returns a phidget handler which can be used other places to communicate
 * with the phidget devices.
 */
PhidgetVoltageInputHandle* voltageInit(int channel) {
	PhidgetVoltageInputHandle ch;
	PhidgetReturnCode res;
	const char *errs;
	res = PhidgetVoltageInput_create(&ch);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to create voltage input channel\n");
		exit(1);
	}

	res = initChannel((PhidgetHandle)ch, channel);
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
