#include <AndyOS.h>
#include <andyos/net/dns.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#define DNS_PORT 53

namespace DNS {
struct DNS_ANSWER
{
    uint16_t name;
    uint16_t type;
    uint16_t clas;
    uint32_t ttl;
    uint16_t length;
    uint32_t addr;
} __attribute__((packed));

struct DNS_HEADER
{
    uint16_t id;
    uint16_t flags;
    uint16_t quest_count;
    uint16_t ans_count;
    uint16_t auth_count;
    uint16_t add_count;
} __attribute__((packed));

struct DNS_PACKET
{
    DNS_HEADER header;
    uint8_t *data;
    int length;
};

struct DNS_TABLE_ENTRY
{
    char *name;
    uint32_t addr;
};

DNS_TABLE_ENTRY cache[DNS_CACHE_SIZE];

bool Decode(DNS_PACKET *dns, void *data, int length)
{
    DNS_HEADER *header = (DNS_HEADER *)data;
    header->id = ntohs(header->id);
    header->flags = ntohs(header->flags);
    header->quest_count = ntohs(header->quest_count);
    header->ans_count = ntohs(header->ans_count);
    header->auth_count = ntohs(header->auth_count);
    header->add_count = ntohs(header->add_count);

    dns->header = *header;
    dns->data = (uint8_t *)header + sizeof(DNS_HEADER);
    dns->length = length - sizeof(DNS_HEADER);
    return true;
}

uint8_t *ParseAnswer(DNS_ANSWER *ans, uint8_t *ptr)
{
    DNS_ANSWER *da = (DNS_ANSWER *)ptr;

    ans->name = ntohs(da->name);
    ans->type = ntohs(da->type);
    ans->clas = ntohs(da->clas);
    ans->ttl = ntohl(da->ttl);
    ans->length = ntohs(da->length);
    ans->addr = da->addr;

    return ptr + sizeof(DNS_ANSWER);
}

uint8_t *ParseName(char *buf, const uint8_t *ptr)
{
    int len = *ptr++;
    while (len--) {
        *buf++ = *ptr++;

        if (!len) {
            if (*ptr) {
                *buf++ = '.';
                len = *ptr++;
            } else {
                *buf++ = *ptr++;
            }
        }
    }

    uint16_t *end = (uint16_t *)ptr;
    uint16_t type = *end++;
    uint16_t clas = *end++;

    return (uint8_t *)end;
}

uint8_t *AppendName(uint8_t *buf, const char *name)
{
    char *ptr = (char *)buf;
    char *head = ptr++;
    const char *p = name;

    while (1) {
        char c = *p++;

        if (c == '.' || c == '\0') {
            int len = ptr - head - 1;
            *head = len;
            head = ptr;
        }

        *ptr++ = c;

        if (!c)
            break;
    };

    return (uint8_t *)ptr;
}

void AddEntry(const char *name, uint32_t addr)
{
    for (int i = 0; i < DNS_CACHE_SIZE; i++) {
        if (!cache[i].name || strcmp(cache[i].name, name) == 0) {
            if (!cache[i].name)
                kprintf("DNS added entry: %s %p\n", name, addr);

            cache[i].name = strdup(name);
            cache[i].addr = addr;
            break;
        }
    }
}

uint32_t Lookup(const char *name)
{
    for (int i = 0; i < DNS_CACHE_SIZE; i++) {
        if (!strcmp(cache[i].name, name)) {
            return cache[i].addr;
        }
    }

    return 0;
}

uint32_t Query(const char *name)
{
    int name_len = strlen(name) + 2;
    int data_len = sizeof(DNS_HEADER) + name_len + 4;
    uint8_t *data = new uint8_t[data_len];

    DNS_HEADER *hdr = (DNS_HEADER *)data;
    hdr->id = htons(42);
    hdr->flags = htons(0x0100);
    hdr->quest_count = htons(1);
    hdr->ans_count = htons(0);
    hdr->auth_count = htons(0);
    hdr->add_count = htons(0);

    AppendName(data + sizeof(DNS_HEADER), name);

    data[sizeof(DNS_HEADER) + name_len + 1] = 1; // type
    data[sizeof(DNS_HEADER) + name_len + 3] = 1; // class

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd <= 0)
        return 0;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(DNS_PORT);
    addr.sin_addr.s_addr = htonl(0xC0A80001);

    int res = sendto(fd, data, data_len, 0, (sockaddr *)&addr, sizeof(addr));
    kprintf("Sent: %d\n", res);

    delete[] data;

    if (res <= 0)
        return 0;

    uint8_t recv_buf[256];
    int recv_len = recvfrom(fd, recv_buf, sizeof(recv_buf), 0, (sockaddr *)&addr, sizeof(addr));
    kprintf("Recv: %d\n", recv_len);

    if (recv_len <= 0)
        return 0;

    kprintf("Received DNS\n");

    DNS_PACKET pkt;
    if (!Decode(&pkt, recv_buf, recv_len))
        return 0;

    char name_buf[256];
    DNS_ANSWER ans;

    uint8_t *ptr = pkt.data;
    ptr = ParseName(name_buf, ptr);
    ptr = ParseAnswer(&ans, ptr);

    AddEntry(name_buf, ans.addr);

    return ans.addr;

    // Send(data);
}
} // namespace DNS
