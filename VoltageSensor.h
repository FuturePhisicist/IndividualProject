#pragma once
#include "Arduino.h"
#include "GyverFilters.h"

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

	float getRealVoltage(int value)
	{
		return constrain(value, 0, 3300);
	};

	float getVoltage()
	{
		int measuredVoltage = getMeasurement();
		int filteredVoltage = voltageFilter.filtered(measuredVoltage);
		// float currentVoltage = getRealVoltage(filteredVoltage);
		return getRealVoltage(filteredVoltage);
	};

	float getInvertedVoltage()
	{
		return 3300 - getVoltage();
	};

private:
	GKalman voltageFilter = GKalman(4095, 0.7);
	int sensorPin;
};