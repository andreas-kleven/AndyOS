#pragma once
#include "Control.h"
#include "string.h"

#define TEXTBOX_MAX_LENGTH 256

namespace gui
{
	class TextBox : public Control
	{
	public:
		String text;

		TextBox();

		virtual void Paint();
		virtual void ReceiveSendMessage(WND_MSG msg);
	};
}