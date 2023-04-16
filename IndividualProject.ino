#include "GyverButton.h"
#include "VoltageSensor.h"
#include "LightSensor.h"
#include "math.h"
const int LIGHT_PIN = 33;
const int VOLTAGE_PIN = 34;
const int WATER_BUTTON_PIN = 35;
const int ELECTROLYTE_BUTTON_PIN = 32;

const double SOIL_VOLUME = 100.0; // ml

// pH dedicated
const double Ka = 100; // H2SO4
const double MolarMass = 98.079; // g/mol, H2SO4

double get_pH(double Hplus)
{
	return -log10(Hplus);
}
// pH dedicated

LightSensor lightSensor(LIGHT_PIN);
VoltageSensor voltageSensor(VOLTAGE_PIN);
GButton waterButton(WATER_BUTTON_PIN);
GButton electrolyteButton(ELECTROLYTE_BUTTON_PIN);

uint32_t tmr = 0;

double currentWaterVolume = 100;
double lastVoltage, lastWaterVolume = 100;

double Hplus = pow(10, -7);

void setup() {  

	Serial.begin(9600);

	waterButton.setTickMode(AUTO);
	electrolyteButton.setTickMode(AUTO);

	lastVoltage = voltageSensor.getInvertedVoltage();
}

void loop() {
	double currentVoltage = voltageSensor.getInvertedVoltage();

	if (waterButton.isClick())
	{
		currentWaterVolume += 100;
		lastWaterVolume = currentWaterVolume;
		lastVoltage = currentVoltage;
	}

	currentWaterVolume = currentVoltage / lastVoltage * lastWaterVolume;

	double soilMosture = currentWaterVolume / SOIL_VOLUME * 100.0;

	// pH dedicated
	if (electrolyteButton.isClick())
	{
		double electrolyteMass = 10; // g
		double electrolyteAmount = electrolyteMass / MolarMass; // mol
		double electrolyteConcentration = electrolyteAmount / 1; // mol/ml // change 1 to addedWaterVolume

		double deltaHplus = -Ka / 2 + sqrt((Ka / 2) * (Ka / 2) + Ka * electrolyteConcentration);

		Hplus += deltaHplus;
	}
	// pH dedicated

	if (millis() - tmr >= 2000) {
		Serial.print(get_pH(Hplus));
		Serial.print(' ');
		Serial.print(soilMosture);		
		Serial.print(' ');
		Serial.println(lightSensor.getLux());
	}
}
