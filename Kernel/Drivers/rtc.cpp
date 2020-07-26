#include <Drivers/rtc.h>
#include <hal.h>

#define CURRENT_YEAR 2018

namespace RTC
{
	int century_reg = 0x00;

	uint8 second;
	uint8 minute;
	uint8 hour;
	uint8 day;
	uint8 month;
	uint32 year;

	int GetUpdateFlag()
	{
		outb(0x70, 0x0A);
		return (inb(0x71) & 0x80);
	}

	unsigned char GetReg(int reg)
	{
		outb(0x70, reg);
		return inb(0x71);
	}

	void ReadRTC()
	{
		uint8 century;
		uint8 last_second;
		uint8 last_minute;
		uint8 last_hour;
		uint8 last_day;
		uint8 last_month;
		uint8 last_year;
		uint8 last_century;
		uint8 registerB;

		while (GetUpdateFlag());

		second = GetReg(0x00);
		minute = GetReg(0x02);
		hour = GetReg(0x04);
		day = GetReg(0x07);
		month = GetReg(0x08);
		year = GetReg(0x09);

		if (century_reg != 0)
		{
			century = GetReg(century_reg);
		}

		do
		{
			last_second = second;
			last_minute = minute;
			last_hour = hour;
			last_day = day;
			last_month = month;
			last_year = year;
			last_century = century;

			while (GetUpdateFlag());

			second = GetReg(0x00);
			minute = GetReg(0x02);
			hour = GetReg(0x04);
			day = GetReg(0x07);
			month = GetReg(0x08);
			year = GetReg(0x09);

			if (century_reg != 0)
			{
				century = GetReg(century_reg);
			}
		} while ((last_second != second) || (last_minute != minute) || (last_hour != hour) ||
			(last_day != day) || (last_month != month) || (last_year != year) ||
			(last_century != century));

		registerB = GetReg(0x0B);

		// Convert BCD to binary values if necessary
		if (!(registerB & 0x04))
		{
			second = (second & 0x0F) + ((second / 16) * 10);
			minute = (minute & 0x0F) + ((minute / 16) * 10);
			hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
			day = (day & 0x0F) + ((day / 16) * 10);
			month = (month & 0x0F) + ((month / 16) * 10);
			year = (year & 0x0F) + ((year / 16) * 10);

			if (century_reg != 0)
			{
				century = (century & 0x0F) + ((century / 16) * 10);
			}
		}

		// Convert 12 hour clock to 24 hour clock if necessary
		if (!(registerB & 0x02) && (hour & 0x80))
		{
			hour = ((hour & 0x7F) + 12) % 24;
		}

		// Calculate the full (4-digit) year
		if (century_reg != 0)
		{
			year += century * 100;
		}
		else
		{
			year += (CURRENT_YEAR / 100) * 100;
			if (year < CURRENT_YEAR) year += 100;
		}
	}

	int Second()
	{
		ReadRTC();
		return second;
	}

	int Minute()
	{
		ReadRTC();
		return minute;
	}

	int Hour()
	{
		ReadRTC();
		return hour;
	}

	int Day()
	{
		ReadRTC();
		return day;
	}

	int Month()
	{
		ReadRTC();
		return month;
	}

	int Year()
	{
		ReadRTC();
		return year;
	}
}