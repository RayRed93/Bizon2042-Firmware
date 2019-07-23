// Bizon2042.h

#include "DateTime.h"
#include <EEPROM.h>


//combine-harvester constants [m]
#define WHEEL_RADIUS 0.7f
#define PINS_COUNT 8
#define INNER_WHEEL_PINS_RADIUS 0.0135f
#define HEADER_WIDTH 4

#define SERIAL_ENABLE true
#define DEBUG false

//lcd
#define LCD_ADDR 0x27
#define LCD_BUFF_WIDTH 21
#define LCD_BUFF_HEIGHT 4
#define LCD_LINE_1 0
#define LCD_LINE_2 1
#define LCD_LINE_3 2
#define LCD_LINE_4 3

//hall sensors
#define HALL_HEADER 2
#define HALL_WHEEL 3

//header
#define HEADER_STATE_RAISED 1
#define HEADER_STATE_DOWN 0

#define ONE_WIRE_BUS 8
#define MAIN_BATTERY A1

//helpers
#define ADC_TO_BOOL(a) (a < 10 ? 0 : 1)

#define GREEN_LED PORTB5		
#define RED_LED PORTB4
#define YELLOW_LED PORTB3

#define BTT_START PORTD5 
#define BTT_STOP PORTD4  
#define BTT_PAUSE PORTB2
#define BTT_RIGHT PORTB1
#define BTT_LEFT PORTD6
#define BTT_F1 A7 //TODO
#define BTT_F2 A6

#ifndef _BIZON2042_h
#define _BIZON2042_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


class Bizon2042Class
{
protected:
	
private:
	void ResetMeasure();
	void LcdBuffer_Active();
	void LcdBuffer_Idle();
	void (*currentScreen)(void);
	char status[10];
	void ClearBuff(char sign);

public:
	char lcdBuff[LCD_BUFF_HEIGHT][LCD_BUFF_WIDTH];
	enum MeasureState
	{
		STARTED,
		STOPPED,
		PAUSED,
		IDLE,
		LOAD,
		SAVE
	};
	Bizon2042Class(bool);
	//char lcdBuff[4][20];
	void SetLed(uint8_t led, bool state);
	void ResetLeds();
	void ButtonStateUpdate();
	void HeaderChanged();
	void WheelTick();
	void FastUpdate();
	void SlowUpdate();
	void Debug();
	float GetProcessedArea();
	float GetDistance();
	void LoadFromEEPROM();
	void SaveToEEPROM();
	void CalculateSpeed(float);
	void ChangeRecordingState(MeasureState state);
	void ClearLCD( void(*f)() );
	
	struct
	{
		bool START;
		bool STOP;
		bool PAUSE;
		bool LEFT;
		bool RIGHT;
		bool F1;
		bool F2;
	}ButtonState;
	
	volatile bool HeaderUp;
	volatile int allWheelTicks;
	volatile int processedWheelTicks;
	float allTraveledDistance; //meters
	int speed; //km/h
	float processedDistance;
	float processedArea;  //ha
	float temperature; //C
	
	DateTime startTime;
	DateTime endTime;
	DateTime workTime;
	DateTime currentTime;
	DateTime stopTime;
	DateTime allStopTime;
	
	MeasureState measureState;
};

extern Bizon2042Class Bizon2042;

#endif

