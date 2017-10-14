#pragma once
#include "Control.h"

namespace gui
{
	class Label : public Control
	{
	public:
		char* text;

		Label();

		virtual void Paint();
	};
}