#include <GyverButton.h>
#include <math.h>
#include <Ds1302.h>
#include <WiFi.h>

#include "VoltageSensor.h"
#include "LightSensor.h"
#include "Settings.h"

const char* SERVER_IP = "192.168.1.71";
const int SERVER_PORT = 8000;
const int POT_ID = 7;

const int RST_PIN = 17;
const int DAT_PIN = 16;
const int CLK_PIN = 4;
const int PUMP_PIN = 27;

const int LIGHT_PIN = 33;
const int VOLTAGE_PIN = 34;
const int WATER_BUTTON_PIN = 35;
const int ELECTROLYTE_BUTTON_PIN = 32;

const double PUMP_PRODUCTIVITY = 100.0; // ml/sec
const int PUMP_WORK_TIME = 100; // sec

const double MIN_MOISTURE = 0.54;
const double SOIL_VOLUME = 100.0; // ml

// pH dedicated
const double Ka = 100; // H2SO4
const double MolarMass = 98.079; // g/mol, H2SO4

double get_pH(double Hplus)
{
	return -log10(Hplus);
}
// pH dedicated

String get_time(const Ds1302::DateTime& now)
{
	String currentTime; 

	if (now.day < 10) currentTime += '0';
	currentTime += now.day;
	currentTime += '-';

	if (now.month < 10) currentTime += '0';
	currentTime += now.month;
	currentTime += '-';

	currentTime += "20";
	currentTime += now.year;
	currentTime += ' ';

	if (now.hour < 10) currentTime += '0';
	currentTime += now.hour;
	currentTime += ':';

	if (now.minute < 10) currentTime += '0';
	currentTime += now.minute;
	currentTime += ':';

	if (now.second < 10) currentTime += '0';
	currentTime += now.second;

	return currentTime;
}

WiFiClient client;
Ds1302 rtc(RST_PIN, CLK_PIN, DAT_PIN);
LightSensor lightSensor(LIGHT_PIN);
VoltageSensor voltageSensor(VOLTAGE_PIN);
GButton waterButton(WATER_BUTTON_PIN);
GButton electrolyteButton(ELECTROLYTE_BUTTON_PIN);

uint32_t tmr = 0;

uint32_t pump_tmr;
bool pumpOn = false;

double currentWaterVolume = 100.0;
double lastVoltage, lastWaterVolume = currentWaterVolume;

double Hplus = pow(10, -7);

void setup() {  
	rtc.init();

	Serial.begin(9600);

	waterButton.setTickMode(AUTO);
	electrolyteButton.setTickMode(AUTO);

	lastVoltage = voltageSensor.getInvertedVoltage();

	pinMode(PUMP_PIN, OUTPUT);

	WiFi.begin(SSID, PASSWORD);
	Serial.println("Connecting to WiFi...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.print("...");
	}
	Serial.println("Connected to WiFi");
}

void loop() {
	double currentVoltage = voltageSensor.getInvertedVoltage();

	currentWaterVolume = currentVoltage / lastVoltage * lastWaterVolume;

	double soilMoisture = currentWaterVolume / SOIL_VOLUME * 100.0;

	// pump dedicated
	Serial.println(pumpOn);
	const uint32_t CURRENT_MILLIS = millis();
	if (not pumpOn and soilMoisture < MIN_MOISTURE) {
		digitalWrite(PUMP_PIN, HIGH);
		pumpOn = true;
		pump_tmr = CURRENT_MILLIS;
	}
	if (pumpOn and CURRENT_MILLIS - pump_tmr >= PUMP_WORK_TIME * 1000) {
		digitalWrite(PUMP_PIN, LOW);
		pumpOn = false;

		// currentWaterVolume += PUMP_WORK_TIME * PUMP_PRODUCTIVITY;
		// lastWaterVolume = currentWaterVolume;
		// lastVoltage = currentVoltage;
	}
	// pump dedicated

	// pH dedicated
	if (electrolyteButton.isClick())
	{
		double electrolyteMass = 10; // g
		double addedWaterVolume = 100; // ml
		double electrolyteAmount = electrolyteMass / MolarMass; // mol
		double electrolyteConcentration = electrolyteAmount / addedWaterVolume; // mol/ml

		double deltaHplus = -Ka / 2 + sqrt((Ka / 2) * (Ka / 2) + Ka * electrolyteConcentration);

		Hplus += deltaHplus;
	}
	// pH dedicated

	Ds1302::DateTime now;
	rtc.getDateTime(&now);

	const String currentTime = get_time(now);

	String data = String(POT_ID);
	data += ' ';
	data += currentTime;
	data += ' ';
	data += get_pH(Hplus);
	data += ' ';
	data += soilMoisture;
	data += ' ';
	data += lightSensor.getLux();

	Serial.println(data);

	if (millis() - tmr >= 10000) {
		if (client.connect(SERVER_IP, SERVER_PORT))
		{
			client.println(data);
			client.flush();
			client.stop();
			Serial.println("Message sent to server");
		} 
		else
		{
			Serial.println("Error connecting to server");
		}
		tmr = millis();
	}
}
