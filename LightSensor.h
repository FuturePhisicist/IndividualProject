#pragma once
#include "Arduino.h"
#include "GyverFilters.h"
#include "templateMap.h"
#include "math.h"

class LightSensor
{
public:
	LightSensor(int pin)
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
		return templateMap(value, 0.0, 4095.0, 0.0, 3.3);
	};

	double getVoltage()
	{
		int measuredVoltage = getMeasurement();
		int filteredVoltage = lightFilter.filtered(measuredVoltage);
		return getRealVoltage(filteredVoltage);
		// return filteredVoltage;
	};

	double getLux()
	{
		double V = getVoltage();
		double RLDR;
		if (V == 0.0)
		{
			RLDR = 0;
		}
		else
		{
			RLDR = R * (Vref - V) / V;
		}
		
		// double lux = LUX_CALC_SCALAR * pow(RLDR, LUX_CALC_EXPONENT);
		// double lux = pow(10.0, (log10(RLDR) - log10(10000.0)) / 5.0);
		// double lux = pow(10.0, (log10(RLDR) - log10(0.000001)) / 5.0);
		double lux = pow(10.0, (log10(RLDR) - log10(0.001)) / 4.7);

		return lux;
	};

	// double getInvertedVoltage()
	// {
	// 	return 3300 - getVoltage();
	// };

private:
	GKalman lightFilter = GKalman(4095, 0.9);
	const double R = 10000;
	const double Vref = 3.3;
	const double LUX_CALC_SCALAR = 12518931;
	const double LUX_CALC_EXPONENT = -1.405;
	int sensorPin;
};