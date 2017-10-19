#pragma once
#include "Control.h"

namespace gui
{
	class Label : public Control
	{
	public:
		Label();

		virtual void Paint();
	};
}