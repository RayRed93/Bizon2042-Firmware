// 
// DateTime by Piotr @Ray Redmerski 
// 

#include "DateTime.h"

DateTime::DateTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t dayOfWeek, uint8_t month, uint8_t year)
{
	this->timeStr = (char*)malloc(sizeof(char) * 8);
	this->dateStr = (char*)malloc(sizeof(char) * 10);
	this->hour = hour;
	this->minute = minute;
	this->second = second;
	this->day = day;
	this->dayOfWeek = dayOfWeek;
	this->month = month;
	this->year = year;
}

DateTime::DateTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	this->timeStr = (char*)malloc(sizeof(char) * 8);
	this->dateStr = (char*)malloc(sizeof(char) * 10);
	this->hour = hour;
	this->minute = minute;
	this->second = second;
	this->day = NULL;
	this->dayOfWeek = NULL;
	this->month = NULL;
	this->year = NULL;
}


DateTime::DateTime()
{
	this->timeStr = (char*)malloc(sizeof(char) * 8);
	this->dateStr = (char*)malloc(sizeof(char) * 10);
	this->hour = NULL;
	this->minute = NULL;
	this->second = NULL;
	this->day = NULL;
	this->dayOfWeek = NULL;
	this->month = NULL;
	this->year = NULL;
}

DateTime::~DateTime()
{
	free(this->timeStr);
	free(this->dateStr);
}

DateTime& DateTime::operator=(const DateTime &dateTime)
{
	this->hour = dateTime.hour;
	this->minute = dateTime.minute;
	this->second = dateTime.second;
	this->day = dateTime.day;
	this->dayOfWeek = dateTime.dayOfWeek;
	this->month = dateTime.month;
	this->year = dateTime.year;
	return *this;
}

DateTime & DateTime::operator+=(const DateTime & dateTime)
{
	this->hour += dateTime.hour;
	this->minute += dateTime.minute;
	this->second += dateTime.second;
	
	if (this->second >= 60)
	{
		++this->minute;
		this->second -= 60;
	}
	if (this->minute >= 60)
	{
		++this->hour;
		this->minute -= 60;
	}
	if (this->hour >= 24)
	{
		++this->day;
		this->hour -= 24;
	}
	return *this;
}

DateTime & DateTime::operator-=(DateTime &dateTime)
{
	if (this->second < dateTime.second)
	{
		--this->minute;
		this->second += 60;
	}
	this->second -= dateTime.second;

	if (this->minute < dateTime.minute)
	{
		--this->hour;
		this->minute += 60;
	}
	this->minute -= dateTime.minute;

	if (this->hour < dateTime.hour)
	{
		--this->day;
		this->hour += 24;
	}
	this->hour -= dateTime.hour;
}

DateTime DateTime::TimeDiffrence(DateTime &timeFrom, DateTime &timeTo)
{
	DateTime diffTime;
	uint8_t t_hour, t_minute, t_second, t_day;

	t_hour = timeTo.hour;
	t_minute = timeTo.minute;
	t_second = timeTo.second;
	t_day = timeTo.day;
	
	
	if (t_second < timeFrom.second)
	{
		--t_minute;
		t_second += 60;
	}
	diffTime.second = t_second - timeFrom.second;

	if (t_minute < timeFrom.minute)
	{
		--t_hour;
		t_minute += 60;
	}
	diffTime.minute = t_minute - timeFrom.minute;

	if (t_hour < timeFrom.hour)
	{
		--t_day;
		t_hour += 24;
	}
	diffTime.hour = t_hour - timeFrom.hour;

	return diffTime;
}

char* DateTime::TimeToStr()
{	
	sprintf(timeStr, "%02d:%02d:%02d", this->hour, this->minute, this->second);
	return timeStr;
}

char* DateTime::DateToStr()
{
	sprintf(dateStr, "%02d/%02d/%d", this->day, this->month, this->year + 2000);
	return dateStr;
}
