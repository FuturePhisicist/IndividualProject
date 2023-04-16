#include "GyverButton.h"
#include "VoltageSensor.h"
#include "math.h"
const int VOLTAGE_PIN = 34;
const int WATER_BUTTON_PIN = 35;
const int ELECTROLYTE_BUTTON_PIN = 32;

const float SOIL_VOLUME = 100.0; // ml

// pH dedicated
const float Ka = 100; // H2SO4
const float MolarMass = 98.079; // g/mol, H2SO4

float get_pH(float Hplus)
{
	return -log10(Hplus);
}
// pH dedicated

VoltageSensor voltageSensor(VOLTAGE_PIN);
GButton waterButton(WATER_BUTTON_PIN);
GButton electrolyteButton(ELECTROLYTE_BUTTON_PIN);

uint32_t tmr = 0;

float currentWaterVolume = 100;
float lastVoltage, lastWaterVolume = 100;

float Hplus = pow(10, -7);

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

	currentWaterVolume = currentVoltage / lastVoltage * lastWaterVolume;

	float soilMosture = currentWaterVolume / SOIL_VOLUME * 100.0;

	// pH dedicated
	if (electrolyteButton.isClick())
	{
		float electrolyteMass = 10; // g
		float electrolyteAmount = electrolyteMass / MolarMass; // mol
		float electrolyteConcentration = electrolyteAmount / 1; // mol/ml // change 1 to addedWaterVolume

		float deltaHplus = -Ka / 2 + sqrt((Ka / 2) * (Ka / 2) + Ka * electrolyteConcentration);

		Hplus += deltaHplus;
	}
	// pH dedicated

	if (millis() - tmr >= 2000) {
		Serial.print(lastWaterVolume);
		Serial.print(' ');
		Serial.print(lastVoltage);
		Serial.print(';');
		Serial.print(currentWaterVolume);
		Serial.print(' ');
		Serial.print(currentVoltage);
		Serial.print(';');
		Serial.print(get_pH(Hplus));
		Serial.print(' ');
		Serial.println(soilMosture);
	}
}
