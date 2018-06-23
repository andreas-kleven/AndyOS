#pragma once

extern "C++"
{
    int set_signal(void(*handler)(int signo));
    void send_signal(int proc_id, int signo);

    int set_message(void(*handler)(int type, char* buf, int size));
    void send_message(int proc_id, int type, char* buf, int size);
}