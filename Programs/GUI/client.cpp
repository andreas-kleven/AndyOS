#include "client.h"
#include <AndyOS.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

namespace gui {
namespace client {

int sockfd = 0;
bool initialized = false;
bool RESID_waiting = false;
void *RESID_buffer = 0;
int RESID_size = 0;

std::vector<Window *> windows;

void AddWindow(Window *wnd)
{
    windows.push_back(wnd);
}

bool SendRequest(int type, int id, const void *in, void *out, int in_size, int out_size)
{
    char buf[512];

    RESID_waiting = out_size > 0;
    RESID_size = out_size;
    RESID_buffer = out;

    int msg_size = in_size + 8;

    if (msg_size > sizeof(buf)) {
        debug_print("Message to big %d %d\n", type, in_size);
        return false;
    }

    memcpy(&buf[0], &type, 4);
    memcpy(&buf[4], &id, 4);
    memcpy(&buf[8], in, in_size);

    if (send(sockfd, buf, msg_size, 0) < 0)
        return false;

    while (RESID_waiting)
        usleep(10000);

    return true;
}

void MessageHandler(const MESSAGE &msg)
{
    if (msg.response) {
        if (!RESID_waiting || !RESID_buffer) {
            debug_print("Response error %d %d %d\n", msg.type, RESID_waiting, RESID_buffer);
            return;
        }

        if (msg.size != RESID_size) {
            debug_print("Response size error %d: %d != %d\n", msg.type, msg.size, RESID_size);
        } else {
            memcpy(RESID_buffer, msg.data, RESID_size);
            RESID_waiting = false;
        }

        return;
    }

    for (int i = 0; i < windows.size(); i++) {
        Window *wnd = windows[i];

        if (wnd->id == msg.id) {
            wnd->HandleMessage(msg);
            break;
        }
    }
}

void *Loop(void *arg)
{
    char buf[512];

    while (true) {
        int len = recv(sockfd, buf, sizeof(buf), 0);
        gui::MESSAGE msg = gui::MESSAGE(buf, len);
        MessageHandler(msg);
    }
}

void Init()
{
    if (initialized)
        return;

    initialized = true;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (sockfd < 0) {
        debug_print("Error %d\n", sockfd);
        return;
    }

    sockaddr_un unixaddr;
    unixaddr.sun_family = AF_UNIX;
    strcpy(unixaddr.sun_path, "/winman-sock");

    if (connect(sockfd, (sockaddr *)&unixaddr, sizeof(unixaddr)) == -1) {
        perror("client connect");
        exit(1);
    }

    connected = true;
    pthread_create(0, 0, Loop, 0);
}

} // namespace client
} // namespace gui
