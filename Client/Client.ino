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

double currentWaterVolume = 100;
double lastVoltage, lastWaterVolume = 100;

double Hplus = pow(10, -7);

void setup() {  
	rtc.init();

	Serial.begin(9600);

	waterButton.setTickMode(AUTO);
	electrolyteButton.setTickMode(AUTO);

	lastVoltage = voltageSensor.getInvertedVoltage();

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

	Ds1302::DateTime now;
	rtc.getDateTime(&now);

	const String currentTime = get_time(now);

	String data = String(POT_ID);
	data += ' ';
	data += currentTime;
	data += ' ';
	data += get_pH(Hplus);
	data += ' ';
	data += soilMosture;
	data += ' ';
	data += lightSensor.getLux();

	// if (millis() - tmr >= 2000) {
	// 	Serial.print(currentTime);
	// 	Serial.print(' ');
	// 	Serial.print(get_pH(Hplus));
	// 	Serial.print(' ');
	// 	Serial.print(soilMosture);		
	// 	Serial.print(' ');
	// 	Serial.println(lightSensor.getLux());
	// }

	if (millis() - tmr >= 10000) {
		if (client.connect(SERVER_IP, SERVER_PORT))
		{
			// client.print(POT_ID);
			// client.print(' ');
			// client.print(currentTime);
			// client.print(' ');
			// client.print(get_pH(Hplus));
			// client.print(' ');
			// client.print(soilMosture);		
			// client.print(' ');
			// client.println(lightSensor.getLux());
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
