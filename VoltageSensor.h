#pragma once
#include "Arduino.h"

class VoltageSensor
{
public:
	VoltageSensor(int pin)
	{
		sensorPin = pin;
		pinMode(sensorPin, INPUT);
	};
	int getMeasurement()
	{
		return analogRead(sensorPin);
	};
private:
	int sensorPin;
};