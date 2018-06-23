#pragma once

struct MESSAGE
{
    int id;
    int type;
    int size;
    char* data;
};

extern "C++"
{
    int set_signal(void(*handler)(int signo));
    void send_signal(int proc_id, int signo);

    int set_message(void(*handler)(int id, int type, char* data, int size));
    void post_message(int proc_id, int type, char* data, int size);
    MESSAGE send_message(int proc_id, int type, char* data, int size);
    void send_message_response(int msg_id, int type, char* data, int size);
}