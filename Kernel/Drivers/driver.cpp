#include <driver.h>
#include <Drivers/ata.h>
#include <Drivers/mouse.h>
#include <string.h>

namespace DriverManager
{
	Driver *first_driver = 0;

	void AddDriver(Driver *driver)
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

	Driver *FirstDriver()
	{
		return first_driver;
	}

	Driver *GetDriver(const char *name)
	{
		Driver *drv = first_driver;

		while (drv)
		{
			if (strcmp(drv->name, name) == 0)
				return drv;

			drv = drv->next;
		}

		return 0;
	}

	Driver *GetDriver(dev_t dev)
	{
		Driver *drv = first_driver;

		while (drv)
		{
			if (drv->dev == dev)
				return drv;

			drv = drv->next;
		}

		return 0;
	}

	STATUS Init()
	{
		ATADriver::Init();
		AddDriver(new MouseDriver());

		return STATUS_SUCCESS;
	}
} // namespace DriverManager
