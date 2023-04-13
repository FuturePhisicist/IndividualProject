#include "GyverFilters.h"
#include "GyverButton.h"
#include "VoltageSensor.h"
const int VOLTAGE_PIN = 34;
const int WATER_BUTTON_PIN = 35;
const int ELECTROLYTE_BUTTON_PIN = 32;

const float WATER_COEFFICIENT = 1.2;
const float ELECTROLYTE_COEFFICIENT = 3.0;

GKalman testFilter(4095, 0.7);
VoltageSensor voltageSensor(VOLTAGE_PIN);
GButton waterButton(WATER_BUTTON_PIN);
GButton electrolyteButton(ELECTROLYTE_BUTTON_PIN);

float getRealVoltage(int value)
{
	// return float(value)/* * 3.3 / 4095.0*/;
  return constrain(value, 0, 3300);
}

uint32_t tmr = 0;

float expectedVoltage = -1.0;
float waterVolume = 0.1;
float electrolyteVolume = 0.0;
float pH = 7.0;

void setup() {

	Serial.begin(9600);

  waterButton.setTickMode(AUTO);
  electrolyteButton.setTickMode(AUTO);
}

void loop() {
	int measuredVoltage = voltageSensor.getMeasurement();
	int filteredVoltage = testFilter.filtered(measuredVoltage);
	float realVoltage = getRealVoltage(filteredVoltage);

	if (waterButton.isClick())
	{
		waterVolume -= 0.01;
		expectedVoltage -= 0.01 * WATER_COEFFICIENT;
	}
	if (electrolyteButton.isClick())
	{
		electrolyteVolume -= 0.005;
		expectedVoltage -= 0.005 * ELECTROLYTE_COEFFICIENT;
	}

	if (waterButton.isHold())
	{
		expectedVoltage = realVoltage;
	}
	if (millis() - tmr >= 2000) {
		Serial.print(realVoltage);
		Serial.print(' ');
		Serial.println(expectedVoltage);
	}
}
