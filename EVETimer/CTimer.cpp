#include "stdafx.h"

Timer::Timer()
{
	m_end = time(0);
	localtime_s(&m_gmEnd, &m_end);
	m_active = false;
}

Timer::Timer(int days, int hours, int minutes, int seconds)
{
	m_end = time(0);
	localtime_s(&m_gmEnd, &m_end);

	addDays(days);
	addHours(hours);
	addMinutes(minutes);
	addSeconds(seconds);
	setTimer();
	m_active = true;

	m_days = days;
	m_hours = hours;
	m_minutes = minutes;
	m_seconds = seconds;
}

Timer::Timer(long stored_time)
{
	m_end = static_cast<time_t>(stored_time);
	localtime_s(&m_gmEnd, &m_end);

	long seconds_to_go = static_cast<long>(difftime(m_end, time(0)));
	seconds_to_go = seconds_to_go < 0 ? -seconds_to_go : seconds_to_go;

	if (seconds_to_go > 0)
	{
		m_active = true;

		// Caculate remaining time
		m_seconds = seconds_to_go % 60;
		seconds_to_go /= 60;
		m_minutes = seconds_to_go % 60;
		seconds_to_go /= 60;
		m_hours = seconds_to_go % 24;
		seconds_to_go /= 24;
		m_days = seconds_to_go;
	}
	else
	{
		m_active = false;
		m_days = m_hours = m_minutes = m_seconds = 0;
	}
}

Timer::~Timer()
{
}

void Timer::addSeconds(int seconds)
{
	m_gmEnd.tm_sec += seconds;
}

void Timer::addMinutes(int minutes)
{
	m_gmEnd.tm_min += minutes;
}

void Timer::addHours(int hours)
{
	m_gmEnd.tm_hour += hours;
}

void Timer::addDays(int days)
{
	m_gmEnd.tm_mday += days;
}

void Timer::setTimer(void)
{
	m_end = mktime(&m_gmEnd);
}

void Timer::setDescr(wchar_t *description)
{
	wcscpy_s(m_description, MAX_STRING, description);
}

void Timer::tick(void)
{
	m_seconds -= 1;
	if (m_seconds == -1)
	{
		m_seconds = 60;
		m_minutes -= 1;
	}
	else
	{
		return;
	}
	
	if (m_minutes == -1)
	{
		m_minutes = 60;
		m_hours -= 1;
	}
	else
	{
		return;
	}

	if (m_hours == -1)
	{
		m_hours = 24;
		m_days -= 1;
	}
	else
	{
		return;
	}

	if (m_days == -1)
	{
		m_days = 0;
		m_hours = 0;
		m_minutes = 0;
		m_seconds = 0;
		m_active = false;
	}
	
	return;
}

long Timer::getStoreTime(void)
{
	return static_cast<long>(m_end);
}

int Timer::getSeconds(void)
{
	return m_seconds;
}

int Timer::getMinutes(void)
{
	return m_minutes;
}

int Timer::getHours(void)
{
	return m_hours;
}

int Timer::getDays(void)
{
	return m_days;
}

BOOL Timer::isActive(void)
{
	return m_active;
}

wchar_t* Timer::getDescr(void)
{
	return m_description;
}

wchar_t * Timer::getDisplayText(void)
{
	swprintf(m_displayText, sizeof(m_displayText) * MAX_STRING, 
		L"%d:%d:%d:%d\t%ws", m_days, m_hours, m_minutes, m_seconds, m_description);
	return m_displayText;
}

wchar_t * Timer::getSaveString(void)
{
	swprintf(m_saveString, sizeof(m_saveString) * MAX_STRING,
		L"%ul:::%ws", static_cast<unsigned long>(m_end), m_description);
	return m_saveString;
}
