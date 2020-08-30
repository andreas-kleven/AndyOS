#pragma once

struct MESSAGE
{
    int id;
    int type;
    int size;
    char *data;
    int src_proc;

    MESSAGE()
    {
        this->type = 0;
        this->src_proc = 0;
    }

    MESSAGE(int type) : MESSAGE() { this->type = type; }

    MESSAGE(int type, void *data, int size) : MESSAGE()
    {
        this->type = type;
        this->size = size;
        this->data = (char *)data;
    }
};

int set_message(MESSAGE (*handler)(MESSAGE msg));
void post_message(int proc_id, int type, void *data, int size);
MESSAGE send_message(int proc_id, int type, void *data, int size);
void send_message_response(int msg_id, int type, void *data, int size);
