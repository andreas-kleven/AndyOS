#include <Kernel/timer.h>
#include <Net/tcpbuffer.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <math.h>
#include <string.h>

void TcpReceiveBuffer::Shutdown()
{
    lock.Aquire();

    closed = true;
    recv_event.Set();

    lock.Release();
}

int TcpReceiveBuffer::Recv(void *buf, size_t len, int flags)
{
    if (!recv_event.WaitIntr())
        return -EINTR;

    lock.Aquire();

    TCP_BUFFER_ENTRY *entry = first_entry;
    size_t written = 0;

    while (entry && written < len) {
        if (entry->data) {
            int copy_size = min(entry->size - entry->pos, len - written);
            char *src = (char *)entry->data + entry->pos;
            char *dst = (char *)buf + written;
            memcpy(dst, src, copy_size);
            written += copy_size;
            entry->pos += copy_size;

            // kprintf("write %d %d\n", entry->seq, copy_size);

            if (entry->pos == entry->size) {
                delete entry->data;
                entry->data = 0;
                entry->size = 0;
            }
        }

        if (entry == current_entry) {
            first_entry = entry;
            first_entry->prev = 0;
            break;
        }

        if (!entry->data) {
            TCP_BUFFER_ENTRY *next = entry->next;
            delete entry;
            entry = next;
            first_entry = entry;
            first_entry->prev = 0;
        }
    }

    if (!closed && (!entry || (entry == current_entry && !entry->data)))
        recv_event.Clear();

    lock.Release();

    return written;
}

bool TcpReceiveBuffer::CanReceive(TCP_Packet *tcp)
{
    return true; // TODO

    if (!first_entry)
        return true;

    return (tcp->header->seq + tcp->data_length - first_entry->next_seq) <= TCP_WINDOW_SIZE;
}

bool TcpReceiveBuffer::HandleReceive(TCP_Packet *tcp, Socket *socket)
{
    if (closed || !tcp || !tcp->data || !tcp->data_length)
        return false;

    if (!CanReceive(tcp))
        return 0;

    uint32 seq = tcp->header->seq;
    uint32 flags = tcp->header->flags;
    uint32 size = tcp->data_length;

    TCP_BUFFER_ENTRY *entry = new TCP_BUFFER_ENTRY();

    entry->data = new char[size];
    memcpy(entry->data, tcp->data, size);

    entry->size = size;
    entry->flags = flags;
    entry->seq = seq;
    entry->next_seq = entry->seq + entry->size;

    lock.Aquire();

    InsertEntryLast(entry);
    recv_event.Set();

    lock.Release();

    return true;
}

int TcpReceiveBuffer::NextSeq()
{
    lock.Aquire();

    int ret = 0;

    if (first_entry) {
        if (current_entry) {
            if (current_entry->next && current_entry->next_seq == current_entry->next->seq) {
                current_entry = current_entry->next;
                ret = current_entry->next_seq;
            }
        } else {
            current_entry = first_entry;
            ret = current_entry->next_seq;
        }
    }

    lock.Release();

    return ret;
}

void TcpReceiveBuffer::InsertEntryLast(TCP_BUFFER_ENTRY *entry)
{
    if (!current_entry) {
        first_entry = entry;
        last_entry = entry;
        entry->next = 0;
        entry->prev = 0;
    } else {
        TCP_BUFFER_ENTRY *cur = last_entry;

        while (cur) {
            if (cur->next_seq == entry->seq) {
                entry->prev = cur;
                entry->next = cur->next;

                if (entry->next)
                    entry->next->prev = entry;

                cur->next = entry;

                if (entry->next)
                    entry->next->prev = entry;
                else
                    last_entry = entry;

                return;
            }

            cur = cur->prev;
        }

        kprintf("Insert TCP entry error\n");
    }
}
