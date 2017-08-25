#pragma once

class OS
{
public:
	static void Main();

	static void ThrowException(char* error, char* msg = "");
};