#include <Ds1302.h>
#include "buildTime.h"

const int RST_PIN = 17;
const int DAT_PIN = 16;
const int CLK_PIN = 4;

Ds1302 rtc(RST_PIN, CLK_PIN, DAT_PIN);

void setup()
{
	delay(300);
	Serial.begin(9600);
	rtc.init();

	Ds1302::DateTime dt;

	dt.year = BUILD_YEAR % 100;
	dt.month = BUILD_MONTH;
	dt.day = BUILD_DAY;

	auto dow = day_of_week(BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
	if (dow == 0)
	{
		dt.dow = 7;	
	}
	else
	{
		dt.dow = dow;
	}
	dt.hour = BUILD_HOUR;
	dt.minute = BUILD_MIN;
	dt.second = BUILD_SEC;

	rtc.setDateTime(&dt);
}

void loop() {
	Ds1302::DateTime now;
	rtc.getDateTime(&now);

	static uint8_t last_second = 0;
	if (last_second != now.second)
	{
		last_second = now.second;

		if (now.day < 10) Serial.print('0');
		Serial.print(now.day);     // 01-31
		Serial.print('-');

		if (now.month < 10) Serial.print('0');
		Serial.print(now.month);   // 01-12
		Serial.print('-');


		Serial.print("20");
		Serial.print(now.year);    // 00-99
		Serial.print(' ');

		if (now.hour < 10) Serial.print('0');
		Serial.print(now.hour);    // 00-23
		Serial.print(':');
		if (now.minute < 10) Serial.print('0');
		Serial.print(now.minute);  // 00-59
		Serial.print(':');
		if (now.second < 10) Serial.print('0');
		Serial.print(now.second);  // 00-59
		Serial.println();
	}
}

byte day_of_week(byte day, unsigned long month, int year)
{
	byte a = (14 - month) / 12;
	year -= a;
	month += 12 * a - 2;
	return (day + (31 * month) / 12 + year + year / 4 - year / 100 + year / 400) % 7;	// 0 - Sunday, 1 - Monday, 6 - Saturday
}