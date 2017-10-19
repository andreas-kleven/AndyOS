#pragma once
#include "definitions.h"
#include "PCI/pci.h"

#define NUM_RX_DESC 256
#define NUM_TX_DESC 256

#define ETH_ALEN 6

typedef size_t(*net_send_packet_t) (struct network_dev *, uint8 *_buf, size_t length);
typedef size_t(*net_receive_packet_t) (struct network_dev *, uint8 *_buf, size_t length);
struct network_dev {
	void *device;
	net_send_packet_t send;
	net_receive_packet_t receive;
	char mac[6];
	uint32 ip;
	uint32 dns_ip;
	uint32 subnet_mask;
	uint32 router[2];
	int resolved;
	struct network_dev *next;
};
struct ether_header
{
	uint8  ether_dhost[ETH_ALEN];  /* destination eth addr */
	uint8  ether_shost[ETH_ALEN];  /* source ether addr    */
	uint16 ether_type;             /* packet type ID field */
};

struct  ether_arp {
	uint16 type;
	uint16 proto;
	uint8 hlen;
	uint8 plen;
	uint16 op;
	uint8 arp_sha[ETH_ALEN]; /* sender hardware address */
	uint8 arp_spa[4];        /* sender protocol address */
	uint8 arp_tha[ETH_ALEN]; /* target hardware address */
	uint8 arp_tpa[4];        /* target protocol address */
};
struct in_addr {
	unsigned long s_addr;  // load with inet_aton()

};

struct sockaddr_in {
	short            sin_family;   // e.g. AF_INET
	unsigned short   sin_port;     // e.g. htons(3490)
	struct in_addr   sin_addr;     // see struct in_addr, below
	char             sin_zero[8];  // zero this if you want to
};

struct ip {
	uint8        ip_hl : 4; /* both fields are 4 bytes */
	uint8        ip_v : 4;
	uint8        ip_tos;
	uint16       ip_len;
	uint16       ip_id;
	uint16       ip_off;
	uint8        ip_ttl;
	uint8        ip_p;
	uint16       ip_sum;
	struct in_addr ip_src;
	struct in_addr ip_dst;
};
extern uint8 our_ip[];
extern uint8 their_ip[];
extern uint8 dest_mac[];

enum proto_type {
	IPPROTO_ICMP = 1,
	IPPROTO_TCP = 6,
	IPPROTO_UDP = 17
};



struct socket {
	struct network_dev *dev;
	int pid;
	//enum sock_type type;
	enum proto_type proto;
	uint16 src_port;
	uint16 dst_port;

};
struct addri {
	int     ai_flags;
	int     ai_family;
	int     ai_socktype;
	int     ai_protocol;
	size_t  ai_addrlen;
	struct  sockaddr *ai_addr;
	char    *ai_canonname;     /* canonical name */
	struct  addrinfo *ai_next; /* this struct can form a linked list */
};
enum transport_type {
	CSOCK_TCP,
	CSOCK_UDP,
	CSOCK_RAW,
	CSOCK_ARP
};
enum build_packet {
	CSOCK_PAYLOAD,
	CSOCK_PHYSICAL,
	CSOCK_INTERNET,
	CSOCK_TRANSPORT
};
struct sockbuf {
	size_t length;
	void *data;
	struct ether_header *eth;

	struct ip *ip;
	struct ether_arp *arp;
	uint16 proto;
	uint32 i_len;

	void *transport;
	uint16 t_proto;
	uint32 t_len;

	void *payload;
	uint32 p_len;

	struct network_dev * dev;
	struct socket *socket;
	char dest_mac[6];
	uint32 dest_ip;
	int broadcast;

	//TAILQ_ENTRY(sockbuf) elem;
};



struct E1000_RX_DESC
{
	uint64 addr;
	uint16 length;
	uint16 checksum;
	uint8 status;
	uint8 errors;
	uint16 special;
};

struct E1000_TX_DESC
{
	uint64 addr;
	uint16 length;
	uint8 cso;
	uint8 cmd;
	uint8 status;
	uint8 css;
	uint16 special;
};

struct e1000
{
	struct network_dev *dev;
	uint16 io_base;
	uint8 *mem_base;
	uint8  mac[6];
	uint8 *rx_free;
	uint8 *tx_free;
	struct E1000_RX_DESC *rx_descs[NUM_RX_DESC];
	struct E1000_TX_DESC *tx_descs[NUM_TX_DESC];
	int is_e;
	uint16 rx_cur;
	uint16 tx_cur;
};

struct network_dev *e1000_init(PCI_CONFIG_SPACE* pciConfigHeader);