// DateTime.h

#ifndef _DATETIME_h
#define _DATETIME_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class DateTime
{
private:
	char * timeStr;
	char * dateStr;
	
public:
	DateTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t dayOfWeek, uint8_t month, uint8_t year);
	DateTime(uint8_t hour, uint8_t minute, uint8_t second);
	DateTime();
	~DateTime();
	DateTime& operator= (const DateTime &dateTime);
	DateTime& operator+= (const DateTime &dateTime);
	DateTime& operator-= (DateTime &dateTime);
	static DateTime TimeDiffrence(DateTime &timeFrom, DateTime &timeTo);
	char * TimeToStr();
	char * DateToStr();
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t day;
	uint8_t dayOfWeek;
	uint8_t month;
	uint8_t year;
};

#endif

