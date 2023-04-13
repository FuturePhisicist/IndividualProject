#include "GyverButton.h"
#include "VoltageSensor.h"
const int VOLTAGE_PIN = 34;
const int WATER_BUTTON_PIN = 35;
const int ELECTROLYTE_BUTTON_PIN = 32;

const float WATER_COEFFICIENT = 1.2;
const float ELECTROLYTE_COEFFICIENT = 3.0;
const float SOIL_VOLUME = 100.0; // ml

VoltageSensor voltageSensor(VOLTAGE_PIN);
GButton waterButton(WATER_BUTTON_PIN);
GButton electrolyteButton(ELECTROLYTE_BUTTON_PIN);

uint32_t tmr = 0;

float expectedVoltage = -1.0;
float currentWaterVolume = 100;
float lastVoltage, lastWaterVolume = 100;
float electrolyteVolume = 0.0;
float pH = 7.0;

void setup() {

	Serial.begin(9600);

	waterButton.setTickMode(AUTO);
	electrolyteButton.setTickMode(AUTO);

	lastVoltage = voltageSensor.getInvertedVoltage();
}

void loop() {
	float currentVoltage = voltageSensor.getInvertedVoltage();

	if (waterButton.isClick())
	{
		currentWaterVolume += 100;
		lastWaterVolume = currentWaterVolume;
		lastVoltage = currentVoltage;
	}
	// if (electrolyteButton.isClick())
	// {
	// 	electrolyteVolume -= 0.005;
	// 	expectedVoltage -= 0.005 * ELECTROLYTE_COEFFICIENT;
	// }

	currentWaterVolume = currentVoltage / lastVoltage * lastWaterVolume;

	float soilMosture = currentWaterVolume / SOIL_VOLUME * 100.0;

	if (millis() - tmr >= 2000) {
		Serial.print(lastWaterVolume);
		Serial.print(' ');
		Serial.print(lastVoltage);
		Serial.print(';');
		Serial.print(currentWaterVolume);
		Serial.print(' ');
		Serial.print(currentVoltage);
		Serial.print(' ');
		Serial.println(soilMosture);
	}
}
