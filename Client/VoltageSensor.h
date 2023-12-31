#pragma once
#include "Arduino.h"
#include "GyverFilters.h"
#include "templateMap.h"

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

	double getRealVoltage(double value)
	{
		return templateMap(value, 0.0, 4095.0, 0.0, 3300.0);
	};

	double getVoltage()
	{
		int measuredVoltage = getMeasurement();
		int filteredVoltage = voltageFilter.filtered(measuredVoltage);
		// double currentVoltage = getRealVoltage(filteredVoltage);
		return getRealVoltage(filteredVoltage);
	};

	double getInvertedVoltage()
	{
		return 3300 - getVoltage();
	};

private:
	GKalman voltageFilter = GKalman(4095, 0.7);
	int sensorPin;
};