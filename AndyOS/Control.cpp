#include "Control.h"
#include "Window.h"
#include "drawing.h"

namespace gui
{
	void Control::Close()
	{
		Control* ctrl = first_child;
		while (ctrl)
		{
			ctrl->Close();
			Control* next = ctrl->next;

			delete ctrl;
			ctrl = next;
		}

		child_count = 0;
		first_child = 0;
		last_child = 0;
	}
}
