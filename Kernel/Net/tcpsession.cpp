#include "tcpsession.h"
#include "tcp.h"
#include "math.h"
#include "HAL/hal.h"
#include "debug.h"

TcpSession::TcpSession()
{
	dst_ip = Net::NullIPv4;
	dst_port = 0;
	src_port = 0;

	state = TCP_CLOSED;
}

void TcpSession::Receive(IPv4_Header* ip_hdr, TCP_Packet* tcp)
{
	TCP_Header* header = tcp->header;

	flags = tcp->header->flags;
	next_seq = header->ack;
	next_ack = header->seq + 1;

	//if (header->flags & PSH)
	//{
	//	Debug::Print("-DATA-");
	//	Debug::Print("%i");
	//	return;
	//}

	Debug::Print("Flags: %ux\tACK: %ui\tSEQ: %ui\n", header->flags, header->ack, header->seq);

	switch (state)
	{
	case TCP_LISTEN:
		break;

	case TCP_SYN_RECEIVED:
		break;

	case TCP_SYN_SENT:
		if (flags & SYN && flags & ACK)
		{
			Debug::Print("Connected\n");

			Send(ACK);
			state = TCP_ESTABLISHED;
		}
		break;

	case TCP_ESTABLISHED:
		if (flags & FIN)
		{
			//if (Send(ACK))
			//{
			//	state = TCP_CLOSE_WAIT;
			//	//Close application
			//
			//	if (Send(FIN))
			//	{
			//		//Debug::Print("FIN\n");
			//		Debug::Print("LAST ACK\n");
			//		state = TCP_LAST_ACK;
			//	}
			//}
			Send(ACK | FIN);
			state = TCP_LAST_ACK;
		}

		if (flags & PSH && flags & ACK)
		{
			ReceivedData(ip_hdr, tcp);
		}
		break;

	case TCP_LAST_ACK:
		if (flags & ACK)
		{
			Debug::Print("Closed\n");
			Reset();
		}
		break;

	case TCP_FIN_WAIT_1:
		if (flags & ACK)
		{
			if (flags & FIN)
				state = TCP_TIME_WAIT;
			else
				state = TCP_FIN_WAIT_2;
		}
		break;

	case TCP_FIN_WAIT_2:
		if (flags & FIN)
		{
			state = TCP_TIME_WAIT;
		}
		break;
	}
}

void TcpSession::Connect(IPv4Address dst, uint16 port)
{
	if (state != TCP_CLOSED)
		return;

	dst_ip = dst;
	dst_port = port;
	src_port = rand();

	if (!Send(SYN))
		return;

	state = TCP_SYN_SENT;

	int t_out = PIT::ticks + 1000;
	while (state == TCP_SYN_SENT && PIT::ticks < t_out)
		_asm pause;

	if (state == TCP_SYN_SENT)
	{
		Close();
		return;
	}
}

void TcpSession::Listen(uint16 port)
{
	if (state != TCP_CLOSED)
		return;

	src_port = port;
	dst_ip = Net::BroadcastIPv4;
	state = TCP_LISTEN;
}

void TcpSession::Close()
{
	//Fix this for all states

	Send(FIN);
	state = TCP_FIN_WAIT_1;

	int t_out = PIT::ticks + 1000;
	while (state == TCP_FIN_WAIT_1 && PIT::ticks < t_out)
		_asm pause;

	if (state == TCP_FIN_WAIT_1)
	{
		//timeout
		Debug::Print("Timeout closed\n");
		Reset();
		return;
	}

	t_out = PIT::ticks + 1000;
	while (state == TCP_FIN_WAIT_2 && PIT::ticks < t_out)
		_asm pause;

	if (state == TCP_TIME_WAIT)
	{
		Send(ACK);
	}

	//delay here?
	Debug::Print("Closed\n");
	Reset();
}

bool TcpSession::SendData(uint8* data, uint32 data_length)
{
	if (state != TCP_ESTABLISHED)
		return 0;

	bool sent = Send(PSH | ACK, data, data_length);
	return sent;
}

void TcpSession::ReceivedData(IPv4_Header* ip_hdr, TCP_Packet* tcp)
{
	next_ack = tcp->header->seq + tcp->data_length;
	Send(ACK);

	Debug::Dump(tcp->data, tcp->data_length, 1);
}

bool TcpSession::Send(uint8 flags, uint8* data, uint32 data_length)
{
	NetPacket* pkt = TCP::CreatePacket(Net::intf, dst_ip, src_port, dst_port, flags, next_seq, next_ack, data, data_length);

	if (!pkt)
		return 0;

	//last_seq = seq;
	//last_ack = ack;

	Net::intf->Send(pkt);
	return 1;
}

void TcpSession::Reset()
{
	flags = 0;
	next_seq = 0;
	next_ack = 0;

	state = TCP_CLOSED;
}
