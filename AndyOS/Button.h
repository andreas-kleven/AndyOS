#pragma once
#include "Control.h"

namespace gui
{
	class Button : public Control
	{
	public:
		char* text;

		Button();

		virtual void Paint();
	};
}