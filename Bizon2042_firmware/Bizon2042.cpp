// 
// Bizon by @Ray  
// 
#include "Bizon2042.h"



char lcdspeed[4];
char tempLcd[5];
char allDistLcd[7];
char areaLcd[6];

float lastTotalDist = 0.;
void printBits(byte myByte)
{
	for (byte mask = B10000000; mask; mask >>= 1) {
		if (mask & myByte)
			Serial.print('1');
		else
			Serial.print('0');
	}
}

void Bizon2042Class::ResetMeasure()
{
	lastTotalDist = 0;
	this->speed = 0;
	this->allWheelTicks = 0;
	this->processedWheelTicks = 0;
	this->allTraveledDistance = 0;
	this->processedDistance = 0;
	this->workTime = DateTime();
	this->allStopTime = DateTime();
	this->stopTime = DateTime();
	this->allStopTime = DateTime();
}

void Bizon2042Class::LcdBuffer_Active()
{
	dtostrf(this->temperature, 4, 1, tempLcd);
	dtostrf((this->allTraveledDistance / 1000.f), 6, 3, allDistLcd);
	dtostrf(this->processedArea, 6, 3, areaLcd);
	areaLcd[5] = '\0';
	
	snprintf(lcdBuff[LCD_LINE_1], LCD_BUFF_WIDTH, "%s  t=%s", currentTime.TimeToStr(), workTime.TimeToStr());
	snprintf(lcdBuff[LCD_LINE_2], LCD_BUFF_WIDTH, "d=%skm  v=%2dkm/h", allDistLcd, this->speed);
	snprintf(lcdBuff[LCD_LINE_3], LCD_BUFF_WIDTH, "P=%sha", areaLcd);
	snprintf(lcdBuff[LCD_LINE_4], LCD_BUFF_WIDTH, "T=%s%cC%11s", tempLcd, (char)223, this->status);
}

void Bizon2042Class::LcdBuffer_Idle()
{	
	dtostrf(this->temperature, 3, 1, tempLcd);
	dtostrf((this->allTraveledDistance / 1000.f), 6, 3, allDistLcd);

	snprintf(lcdBuff[LCD_LINE_1], LCD_BUFF_WIDTH, "%s  %s", currentTime.TimeToStr(), currentTime.DateToStr());
	snprintf(lcdBuff[LCD_LINE_2], LCD_BUFF_WIDTH, "%s", "--------------------");
	snprintf(lcdBuff[LCD_LINE_3], LCD_BUFF_WIDTH, "d=%skm  v=%2dkm/h", allDistLcd, this->speed);
	snprintf(lcdBuff[LCD_LINE_4], LCD_BUFF_WIDTH, "Temperatura:  %s%cC", tempLcd, (char)223);
}

void Bizon2042Class::ClearBuff(char sign)
{
	memset(lcdBuff[LCD_LINE_1], sign, LCD_BUFF_WIDTH - 1);
	memset(lcdBuff[LCD_LINE_2], sign, LCD_BUFF_WIDTH - 1);
	memset(lcdBuff[LCD_LINE_3], sign, LCD_BUFF_WIDTH - 1);
	memset(lcdBuff[LCD_LINE_4], sign, LCD_BUFF_WIDTH - 1);
}

Bizon2042Class::Bizon2042Class(bool headerState)
{
	this->HeaderUp = headerState; //Podniesiony przy starcie???
	this->ChangeRecordingState(MeasureState::IDLE);
	this->currentTime = DateTime();
	this->startTime = DateTime();
	
	this->workTime = DateTime();
	this->allStopTime = DateTime();
	this->stopTime = DateTime();
	this->allTraveledDistance = 0.;
	//memset(status,0, sizeof status);
	strcpy(status, "<IDLE>");
	
	//this->lcdBuff = (char*)malloc(sizeof(char) * 81);
	ClearBuff(' ');

#pragma region I/O setup
	byte maskb = B00000000;
	maskb |= ((1 << BTT_RIGHT) | (1 << BTT_PAUSE));
	DDRB &= ~maskb; //input
	PORTB |= maskb; //pullup

	byte maskd = B00000000;
	maskd |= ((1 << BTT_LEFT) | (1 << BTT_START) | (1 << BTT_STOP));
	DDRD &= ~maskd; //input
	PORTD |= maskd; //pullup

	byte led_mask = B00000000;
	led_mask |= ((1 << RED_LED) | (1 << GREEN_LED) | (1 << YELLOW_LED));
	DDRB |= led_mask; //output  

	//pinMode(BTT_F1, INPUT_PULLUP);
	//pinMode(BTT_F2, INPUT_PULLUP);


#pragma endregion
}

void Bizon2042Class::SetLed(uint8_t led, bool state)
{
	byte mask = 1 << led;
	if (state)
	{
		PORTB |= mask;
	}
	else
	{
		PORTB &= ~mask;
	}
}

void Bizon2042Class::ResetLeds()
{
	PORTB &= ~((1 << GREEN_LED) | (1 << RED_LED) | (1 << YELLOW_LED));
}

void Bizon2042Class::ButtonStateUpdate()
{
	byte pinb = PINB;
	byte pind = PIND;
	
	this->ButtonState.LEFT = pind & (1 << BTT_LEFT);
	this->ButtonState.RIGHT = pinb & (1 << BTT_RIGHT);
	this->ButtonState.PAUSE = pinb & (1 << BTT_PAUSE);
	//this->ButtonState.F1 = ADC_TO_BOOL(analogRead(BTT_F1));
	//this->ButtonState.F2 = digitalRead(BTT_F2);
	this->ButtonState.START = pind & (1 << BTT_START);	
	this->ButtonState.STOP = pind & (1 << BTT_STOP);
}

void Bizon2042Class::HeaderChanged()
{
	if (HeaderUp == HEADER_STATE_RAISED) HeaderUp = HEADER_STATE_DOWN;
	else HeaderUp = HEADER_STATE_RAISED;

	
	if (measureState == MeasureState::STARTED || measureState == MeasureState::PAUSED)
	{
		if (HeaderUp == HEADER_STATE_RAISED)
		{
			ChangeRecordingState(MeasureState::PAUSED);
			memset(status, 0, sizeof status);
			strcpy(status, " <PAUSE>");
			SetLed(YELLOW_LED, HIGH);
		}
		else
		{
			ChangeRecordingState(MeasureState::STARTED);
			memset(status, 0, sizeof status);
			strcpy(status, "<RUNNING>");
			SetLed(GREEN_LED, HIGH);
			//todo back to ChangeRecordingState????
		}
	}

	
}

void Bizon2042Class::WheelTick()
{
	if (this->measureState == MeasureState::STARTED || this->measureState == MeasureState::IDLE || this->measureState == MeasureState::PAUSED)
	{
		this->allWheelTicks++;
		if (this->HeaderUp == HEADER_STATE_DOWN && this->measureState == MeasureState::STARTED)
		{
			this->processedWheelTicks++;
		}
	}
}

void Bizon2042Class::FastUpdate()
{
	

	if (this->measureState == MeasureState::IDLE) //first start
	{
		
		if (!this->ButtonState.START)
		{

			ResetMeasure();
			ClearBuff('\0');
			this->ChangeRecordingState(MeasureState::STARTED);
			Serial.println("<RUNNING>");
	
			this->startTime = this->currentTime;
			Serial.println(startTime.TimeToStr());
			Serial.println("STARTED WITH NEW TIME");

			SetLed(GREEN_LED, HIGH);
			memset(status, 0, sizeof status);
			strcpy(status, "<RUNNING>");
			//currentScreen = this->LcdBuffer_Active;
			
			
			//delay(400);
			return;
		}
	}


	if (this->measureState != MeasureState::STARTED && this->measureState != MeasureState::IDLE)  //continue
	{

		if (!this->ButtonState.START)
			//ClearBuff(' ');
		{
			if (this->measureState == MeasureState::STOPPED)
			{
				allStopTime += DateTime::TimeDiffrence(stopTime, currentTime);
			}
			this->ChangeRecordingState(MeasureState::STARTED);
			SetLed(GREEN_LED, HIGH);
			
			//delay(400);
		
			Serial.println("<CONTINUE>");
			memset(status, 0, sizeof status);
			strcpy(status, "<RUNNING>");
			return;
		}
	}

	if (this->measureState == MeasureState::STARTED)
	{
		if (!this->ButtonState.PAUSE)
		{
			this->ChangeRecordingState(MeasureState::PAUSED);
			SetLed(YELLOW_LED, HIGH);
			//allStopTime += DateTime::TimeDiffrence(stopTime, currentTime);
			memset(status, 0, sizeof status);
			strcpy(status, " <PAUSE>");
			
			return;
		}

		if (!this->ButtonState.RIGHT) // LOAD LAST SAVE
		{
			this->ResetMeasure();
			ClearBuff('\0');
			LoadFromEEPROM();
			this->ChangeRecordingState(MeasureState::STOPPED);
			this->stopTime = currentTime;
			SetLed(RED_LED, HIGH);
			//return;
			Serial.println("<STOP>");
			memset(status, 0, sizeof status);
			strcpy(status, "<STOP>");
			Serial.println("loaded from eeprom!");
		}
	}

	if (this->measureState == MeasureState::PAUSED || this->measureState == MeasureState::STARTED)
	{
		if (!this->ButtonState.STOP)
		{

			this->ChangeRecordingState(MeasureState::STOPPED);
			this->stopTime = currentTime;
			SetLed(RED_LED, HIGH);
			//return;
			Serial.println("<STOP>");
			memset(status, 0, sizeof status);
			strcpy(status, "<STOP>");
			return;
		}
	}

	if (this->measureState == MeasureState::STOPPED)
	{
		if (!this->ButtonState.LEFT)
		{
			SaveToEEPROM();
			Serial.println("saved to eeprom!");
			this->ChangeRecordingState(MeasureState::IDLE);
			
			SetLed(RED_LED, LOW);
			this->endTime = this->currentTime;
			Serial.println("<TO IDLE>");
			//memset(status, 0, sizeof status);
			strcpy(status, "<IDLE>");
	
		
			//save to eprrom?
			this->ResetMeasure();
			
			return;
		}

		
	}
}
	
void Bizon2042Class::SlowUpdate()
{
	switch (this->measureState)
	{
	case MeasureState::STARTED:
		//sprintf(lcdBuff[LCD_LINE_1],"")
		GetDistance();
		GetProcessedArea();
		workTime = DateTime::TimeDiffrence(startTime, currentTime);
		workTime -= allStopTime;
		Serial.println(allStopTime.TimeToStr());
		this->LcdBuffer_Active();
			
		break;
	case MeasureState::STOPPED:
		GetDistance();
		//GetProcessedArea();
		//workTime = workTime.TimeDiffrence(startTime, currentTime);
		//workTime -= allStopTime;
		this->LcdBuffer_Active();
		Serial.println(allStopTime.TimeToStr());

		break;
	case MeasureState::PAUSED:
		GetDistance();
		GetProcessedArea();
		workTime = DateTime::TimeDiffrence(startTime, currentTime);
		workTime -= allStopTime;
		//workTime -= allStopTime;
		this->LcdBuffer_Active();

		break;
	case MeasureState::IDLE:
		GetDistance();
		this->LcdBuffer_Idle();
		break;
	default:
		break;
	}
	
}

void Bizon2042Class::Debug()
{
	char msg_buttons[100];
	char msg_variables[100];
	char alldist[6];
	char procdist[6];
	char area[6];
	
	dtostrf(allTraveledDistance, 4, 2, alldist);
	dtostrf(processedDistance, 4, 2, procdist);
	dtostrf(processedArea, 4, 2, area);
	
	sprintf(msg_buttons, "Buttons: START:%d STOP:%d PAUSE:%d LEFT:%d RIGHT:%d F1:%d F2:%d ",
		ButtonState.START, ButtonState.STOP, ButtonState.PAUSE, ButtonState.LEFT, ButtonState.RIGHT, ButtonState.F1, ButtonState.F2);
	sprintf(msg_variables, "allTicks:%d procTicks:%d allDist:%s procDist:%s area:%s workTim: %d", 
		allWheelTicks, processedWheelTicks, alldist, procdist, area, 0);
	Serial.print(msg_buttons);
	Serial.println(msg_variables);
}

float Bizon2042Class::GetProcessedArea()
{
	float processedArea;
	float distance;
	distance = (TWO_PI * WHEEL_RADIUS) / PINS_COUNT * processedWheelTicks * 0.0001;
	processedArea = distance * HEADER_WIDTH;
	this->processedArea = processedArea;
	return processedArea;
}

float Bizon2042Class::GetDistance()
{
	float distance = (TWO_PI * WHEEL_RADIUS) / PINS_COUNT * allWheelTicks;
	this->allTraveledDistance = distance;
	return distance;
}

void Bizon2042Class::LoadFromEEPROM()
{
	noInterrupts();
	int addr = 0;
	
	int wheelTicksAll;
	EEPROM.get<int>(addr, wheelTicksAll);
	this->allWheelTicks = wheelTicksAll;
	addr += sizeof(int);
	
	int wheelTicksProcessed;
	EEPROM.get<int>(addr, wheelTicksProcessed);
	this->processedWheelTicks = wheelTicksProcessed;
	addr += sizeof(int);

	bool headerUp;
	EEPROM.get<bool>(addr, headerUp);
	this->HeaderUp = headerUp;
	addr += sizeof(bool);

	EEPROM.get<MeasureState>(addr, this->measureState);
	addr += sizeof(MeasureState);

	EEPROM.get<DateTime>(addr, this->allStopTime);
	addr += sizeof(DateTime);

	EEPROM.get<DateTime>(addr, this->startTime);
	addr += sizeof(DateTime);

	EEPROM.get<DateTime>(addr, this->stopTime);
	addr += sizeof(DateTime);

	EEPROM.get<DateTime>(addr, this->workTime);
	addr += sizeof(DateTime);

	EEPROM.get<DateTime>(addr, this->endTime);
	addr += sizeof(DateTime);
	
	
	allStopTime += DateTime::TimeDiffrence(this->stopTime, this->currentTime);

	GetDistance();
	GetProcessedArea();
	interrupts();
}

void Bizon2042Class::SaveToEEPROM()
{
	noInterrupts();
	for (int i = 0; i < 300; i++) //clear first bytes of eeprom 
	{
		EEPROM.write(i, 0);
	}
	int addr = 0;
	EEPROM.put<int>(addr, (int)this->allWheelTicks);
	addr += sizeof(int);
	EEPROM.put<int>(addr, (int)this->processedWheelTicks);
	addr += sizeof(int);
	EEPROM.put<bool>(addr, (bool)this->HeaderUp);
	addr += sizeof(bool);
	//EEPROM.put<float>(addr, lastTotalDist);
	//addr += sizeof(float);
	EEPROM.put<MeasureState>(addr, this->measureState);
	addr += sizeof(MeasureState);
	EEPROM.put<DateTime>(addr, this->allStopTime);
	addr += sizeof(DateTime);
	EEPROM.put<DateTime>(addr, this->startTime);
	addr += sizeof(DateTime);
	EEPROM.put<DateTime>(addr, this->stopTime);
	addr += sizeof(DateTime);
	EEPROM.put<DateTime>(addr, this->workTime);
	addr += sizeof(DateTime);	
	EEPROM.put<DateTime>(addr, this->endTime);
	addr += sizeof(DateTime);
	interrupts();
}

void Bizon2042Class::CalculateSpeed(float time)
{
	
	if (lastTotalDist > 0)
	{	
		float distanceDiff = (GetDistance() - lastTotalDist) / 1000.f;
		float speed = distanceDiff / (time / 3600.f);
		this->speed = round(speed);
		Serial.print("speed: ");
		Serial.println(speed);
	}
	else
	{
		this->speed = 0.0f;
	}
	lastTotalDist = GetDistance();
}

void Bizon2042Class::ChangeRecordingState(MeasureState state) // TODO....
{
	this->measureState = state;
	
	ResetLeds();
}

void Bizon2042Class::ClearLCD(void(*f)())
{
	(*f)();
}


