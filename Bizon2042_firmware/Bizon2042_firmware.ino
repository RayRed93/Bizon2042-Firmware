/*
 Name:		Bizon2042_firmware.ino
 Created:	7/26/2017 10:43:40 AM
 Author:	Piotr
*/

// the setup function runs once when you press reset or power the board

//#include "DateTime.h"
//#include <TimerThree.h>
#include <OneWire.h>
#include <TimerOne.h>

#include "DateTime.h"
#include "Bizon2042.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <BigFont.h>
#include <uRTCLib.h>
#include <DallasTemperature.h>
//#include <TimerOne.h>
char empty[20];
//DateTime time1(14,22,32), time2(15,11,3);
#define SCREEN_REFRESH_TIME 10000000
#define MAIN_UPDATE_INTERVAL 1000
#define SPEED_CALCULATE_INTERVAL 2000
Bizon2042Class bizon(HEADER_STATE_DOWN);

OneWire wire(ONE_WIRE_BUS);
uRTCLib rtc;
LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);
DallasTemperature ds_temp(&wire);

Bizon2042Class::MeasureState cMeasure = Bizon2042Class::MeasureState::IDLE;
//
//OneWire wire(ONE_WIRE_BUS);
//uRTCLib rtc;
//LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);
//DallasTemperature ds_temp(&wire);
byte arrowLeft[8] = {
	0b00011,
	0b00111,
	0b01111,
	0b11111,
	0b01111,
	0b00111,
	0b00011,
	0b00000
};
byte arrowRight[8] = {
	0b11000,
	0b11100,
	0b11110,
	0b11111,
	0b11110,
	0b11100,
	0b11000,
	0b00000
};

void setup() 
{	
	Serial.begin(115200);
	
	lcd.begin(20, 4);
	lcd.init();
	lcd.backlight();
	lcd.clear();
	ds_temp.begin();
	
	lcd.createChar(0, arrowLeft);
	lcd.createChar(1, arrowRight);
	
	pinMode(HALL_WHEEL, INPUT_PULLUP);
	pinMode(HALL_HEADER, INPUT_PULLUP);



	attachInterrupt(digitalPinToInterrupt(HALL_HEADER), &HeaderChange, CHANGE);
	attachInterrupt(digitalPinToInterrupt(HALL_WHEEL), &WheelTick, RISING);
	
	lcd.setCursor(1, 1);
	lcd.printstr("Bizon System v1.04");
	lcd.setCursor(0, 3);
	lcd.printstr("Loading...");


	
	
	delay(300);
	bizon.SetLed(RED_LED, HIGH);
	delay(300);
	bizon.SetLed(YELLOW_LED, HIGH);
	delay(300);
	bizon.SetLed(GREEN_LED, HIGH);

	delay(1200);
	lcd.clear();

	bizon.SetLed(RED_LED, LOW);
	bizon.SetLed(YELLOW_LED, LOW);
	bizon.SetLed(GREEN_LED, LOW);

	bizon.ButtonStateUpdate();
	if (!bizon.ButtonState.STOP)
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.printstr("SERIAL TIME SET MODE");

		SetTimeFromSerial();

		delay(5000);
	}

	
	//DateTime diff = DateTime::TimeDiffrence(time1, time2);
	//lcd.print(diff.TimeToStr());
	//Serial.print(time1.hour+"_");
	/*Serial.print(time1.second);
	Serial.println();
	Serial.print(time1.minute);
	Serial.println();
	Serial.print(time1.hour);
	Serial.println(time1.TimeToStr());*/
	//Serial.print(time1.second+"_");
	//Serial.println("READY 1");
	memset(empty, ' ', 20);
	delay(100);
	//Timer1.initialize();
	//Timer1.attachInterrupt(&FastUpdate,200000);
	//Timer3.initialize(400000);
	//Timer1.attachInterrupt(&BizonUpdateCallback, 400000);

	//rtc.set(0, 11, 15, 7, 7, 7, 19);

	//sprintf(bizon.lcdBuff[1], "TEST");

	
	//Serial.println("READY 2");
}

void SetTimeFromSerial()
{
	Serial.println("Bizon System v1.04\nEnter time and date!");
	Serial.print("second=");
	while(Serial.available() == 0) {}
	byte second = Serial.readStringUntil('\n').toInt();
	Serial.print(second);
	Serial.print("\nminute=");
	while (Serial.available() == 0) {}
	byte minute = Serial.readStringUntil('\n').toInt();
	Serial.print(minute);
	Serial.print("\nhour=");
	while (Serial.available() == 0) {}
	byte hour = Serial.readStringUntil('\n').toInt();
	Serial.print(hour);
	Serial.print("\nday=");
	while (Serial.available() == 0) {}
	byte day = Serial.readStringUntil('\n').toInt();
	Serial.print(day);
	Serial.print("\ndayofWeek=");
	while (Serial.available() == 0) {}
	byte dayOfWeek = Serial.readStringUntil('\n').toInt();
	Serial.print(dayOfWeek);
	Serial.print("\nmonth=");
	while (Serial.available() == 0) {}
	byte month = Serial.readStringUntil('\n').toInt();
	Serial.print(month);
	Serial.print("\nyear[last 2 digits]=");
	while (Serial.available() == 0) {}
	byte year = Serial.readStringUntil('\n').toInt();
	Serial.print(year);

	rtc.set(second, minute, hour, dayOfWeek, day, month, year);
	//rtc.eeprom_write<long>(2, 2000);
	//eeprom_write
	Serial.println("\nTime and date seted!");
	lcd.setCursor(0, 2);
	lcd.printstr("\nTime and date seted!");


}


void BizonUpdateCallback()
{
	
	digitalWrite(RED_LED, !digitalRead(RED_LED));
	//lcd.setCursor(2, 2);
	//lcd.printstr("DUPA");
	//Serial.print("callback");
	//Serial.println(millis());
}

void HeaderChange()
{
	bizon.HeaderChanged();
}

void WheelTick()
{
	bizon.WheelTick();
}
void WritePageToLCD(char buff[LCD_BUFF_HEIGHT][LCD_BUFF_WIDTH])
{
	//if (lcd.availableForWrite())
	{
		lcd.setCursor(0, LCD_LINE_1);
		lcd.print(buff[LCD_LINE_1]);
		lcd.setCursor(0, LCD_LINE_2);
		lcd.print(buff[LCD_LINE_2]);
		lcd.setCursor(0, LCD_LINE_3);
		lcd.print(buff[LCD_LINE_3]);
		lcd.setCursor(0, LCD_LINE_4);
		lcd.print(buff[LCD_LINE_4]);
	}
	
	//memset(bizon.lcdBuff, 0, sizeof bizon.lcdBuff);
}
int counter = 0;
long lastMainTime = 0;
long velocityLastTime = 0;

void loop() 
{	
	bizon.ButtonStateUpdate();	

	if (millis() - lastMainTime > MAIN_UPDATE_INTERVAL)
	{
		lastMainTime = millis();
		rtc.refresh();

		ds_temp.requestTemperaturesByIndex(0);
		float temp = ds_temp.getTempCByIndex(0);
		bizon.currentTime = DateTime(rtc.hour(), rtc.minute(), rtc.second(), rtc.day(), rtc.dayOfWeek(), rtc.month(), rtc.year());

		if (temp == (-127))
		{
			bizon.temperature = 0;
		}
		else
		{
			bizon.temperature = temp;
		}
		//bizon.temperature = temp;


		bizon.FastUpdate();
		bizon.SlowUpdate();

		if (millis() - velocityLastTime > SPEED_CALCULATE_INTERVAL)
		{
			velocityLastTime = millis();
			bizon.CalculateSpeed(SPEED_CALCULATE_INTERVAL / 1000);
		}



		if (cMeasure != bizon.measureState && (bizon.measureState == Bizon2042Class::MeasureState::IDLE || bizon.measureState == Bizon2042Class::MeasureState::STARTED))
		{
			cMeasure = bizon.measureState;
			lcd.clear();
		}
		WritePageToLCD(bizon.lcdBuff);


		//bizon.Debug();
		
	}
	
	
		
}


