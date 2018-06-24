#pragma once

class Client
{
public:
    static void Init();

    static int CreateWindow(char* title);

private:
    template <class IN, class OUT>
    static bool SendRequest(IN req, OUT& res, bool ignore_disconnected = false);
};