#pragma once

struct MESSAGE
{
    int id;
    int type;
    int size;
    char* data;

    MESSAGE()
    {
        this->type = 0;
    }

    MESSAGE(int type)
    {
        this->type = type;
    }

    MESSAGE(int type, void* data, int size)
    {
        this->type = type;
        this->size = size;
        this->data = (char*)data;
    }
};

extern "C++"
{
    int set_signal(void(*handler)(int signo));
    void send_signal(int proc_id, int signo);

    int set_message(MESSAGE(*handler)(MESSAGE msg));
    void post_message(int proc_id, int type, void* data, int size);
    MESSAGE send_message(int proc_id, int type, void* data, int size);
    void send_message_response(int msg_id, int type, void* data, int size);
}