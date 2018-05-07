#pragma once
#include "phidget22.h"

PhidgetRCServoHandle* servoInit();
void startServo(PhidgetRCServoHandle* ch);
