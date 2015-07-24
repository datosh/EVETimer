#pragma once

#define MAX_STRING 256

class Timer
{
public:
	// Standard Constructor
	Timer();
	// Constructor that is used to create a timer from days, hours, minutes, seconds
	Timer(int, int, int, int);
	// Constructor that is used to create timer from stored seconds
	Timer(long);
	~Timer();

	void addSeconds(int);
	void addMinutes(int);
	void addHours(int);
	void addDays(int);
	void setTimer(void);
	void setDescr(wchar_t*);
	void tick(void);

	long getStoreTime(void);
	int getSeconds(void);
	int getMinutes(void);
	int getHours(void);
	int getDays(void);
	BOOL isActive(void);
	wchar_t* getDescr(void);
	wchar_t* getDisplayText(void);
	wchar_t* getSaveString(void);

private:
	time_t m_end;
	struct tm m_gmEnd;
	int m_days, m_hours, m_minutes, m_seconds;
	BOOL m_active;
	wchar_t m_description[MAX_STRING];
	wchar_t m_displayText[MAX_STRING];
	wchar_t m_saveString[MAX_STRING];
};


