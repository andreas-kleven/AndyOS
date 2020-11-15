#pragma once
#include <Net/socket.h>
#include <Net/tcptypes.h>
#include <sync.h>
#include <types.h>

struct TCP_BUFFER_ENTRY
{
    void *data;
    size_t size;
    uint32 flags;
    uint32 seq;
    uint32 next_seq;
    uint32 pos;
    TCP_BUFFER_ENTRY *next;
    TCP_BUFFER_ENTRY *prev;
};

class TcpReceiveBuffer
{
  public:
    void Shutdown();
    int Recv(void *buf, size_t len, int flags);
    bool CanReceive(TCP_Packet *tcp);
    bool HandleReceive(TCP_Packet *tcp, Socket *socket);
    int NextSeq();

  private:
    bool closed = false;
    Mutex lock;
    Event recv_event;
    TCP_BUFFER_ENTRY *first_entry = 0;
    TCP_BUFFER_ENTRY *last_entry = 0;
    TCP_BUFFER_ENTRY *current_entry = 0;

    void InsertEntryLast(TCP_BUFFER_ENTRY *entry);
};
