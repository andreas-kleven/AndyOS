#include "driver.h"
#include "ata.h"
#include "mouse.h"
#include "string.h"

namespace DriverManager
{
	Driver* first_driver = 0;

	void AddDriver(Driver* driver)
	{
		if (!driver)
			return;

		if (driver->status == DRIVER_STATUS_ERROR)
			return;

		if (first_driver)
			driver->next = first_driver;
		else
			driver->next = 0;

		first_driver = driver;
	}

	Driver* GetDriver()
	{
		return first_driver;
	}

	Driver* GetDriver(const char* id)
	{
		Driver* drv = first_driver;

		while (drv)
		{
			if (strcmp(drv->id, id) == 0)
				return drv;

			drv = drv->next;
		}

		return drv;
	}
	
	STATUS Init()
	{
		ATADriver::Init();
		AddDriver(new MouseDriver());

		return STATUS_SUCCESS;
	}
}