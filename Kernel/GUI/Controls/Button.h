#pragma once
#include "Control.h"

namespace gui
{
	class Button : public Control
	{
	public:
		Button();

		virtual void Paint();
		virtual void ReceiveSendMessage(WND_MSG msg);

	private:
		bool hover = 0;
		bool pressed = 0;
	};
}