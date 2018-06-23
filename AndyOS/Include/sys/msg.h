#pragma once

extern "C++"
{
    int set_signal(void(*handler)(int signo));
    void send_signal(int proc_id, int signo);
}