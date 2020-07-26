#include <Net/tcp.h>
#include <Net/socketmanager.h>
#include <math.h>
#include <errno.h>
#include <debug.h>

TcpSession::TcpSession()
{
	ack_event = Event(false, true);
	fin_event = Event(false, true);
	Reset();
}

int TcpSession::Connect(const sockaddr_in *addr)
{
	mutex.Aquire();
	debug_print("Connecting...\n");

	if (state != TCP_CLOSED)
	{
		mutex.Release();
		return -1;
	}

	socket->addr = (sockaddr *)(new sockaddr_in(*addr));

	state = TCP_SYN_SENT;

	if (!SendWait(TCP_SYN))
	{
		mutex.Release();
		return -ETIMEDOUT;
	}

	if (state != TCP_ESTABLISHED)
	{
		Reset();
		mutex.Release();
		return -ECONNREFUSED;
	}

	debug_print("Connected\n");
	mutex.Release();
	return 0;
}

int TcpSession::Close(int how)
{
	mutex.Aquire();

	debug_print("Closing...\n");

	if (state == TCP_CLOSED)
		return -1;
	// else if (state == ...)

	if (state == TCP_CLOSE_WAIT)
	{
		state = TCP_LAST_ACK;
		SendWait(TCP_FIN | TCP_ACK);
		debug_print("Closed\n");
		Reset();
		socket->HandleClose();
	}
	else
	{
		state = TCP_FIN_WAIT_1;

		if (!SendWait(TCP_FIN | TCP_ACK))
		{
			mutex.Release();
			return 0;
		}

		if (state == TCP_FIN_WAIT_2)
			fin_event.Wait(TCP_TIMEOUT);

		if (state == TCP_TIME_WAIT)
		{
			// TODO: delay
			debug_print("Wait closed\n");
			Reset();
			socket->HandleClose();
			mutex.Release();
		}
		else
		{
			debug_print("Closed with state: %d\n", state);
			Reset();
			socket->HandleClose();
			mutex.Release();
		}
	}

	return 0;
}

Socket *TcpSession::Accept(const sockaddr_in *addr, int flags)
{
	while (state == TCP_LISTEN)
	{
		session_event.Wait();
		sessions_mutex.Aquire();

		if (new_sockets.Count() > 0)
		{
			Socket *new_socket = new_sockets.Dequeue();

			if (new_sockets.Count() == 0)
				session_event.Clear();

			sessions_mutex.Release();

			if (new_socket->tcp_session->SendWait(TCP_SYN | TCP_ACK))
				return new_socket;
		}
		else
		{
			sessions_mutex.Release();
		}
	}

	return 0;
}

int TcpSession::Listen(int backlog)
{
	mutex.Aquire();

	if (state != TCP_CLOSED)
	{
		mutex.Release();
		return -1;
	}

	this->backlog = backlog;
	state = TCP_LISTEN;
	socket->listening = true;
	mutex.Release();
	return 0;
}

int TcpSession::SendData(const void *buf, size_t len, int flags)
{
	mutex.Aquire();

	if (state != TCP_ESTABLISHED)
	{
		mutex.Release();
		return -1;
	}

	if (SendWait(TCP_PSH | TCP_ACK, buf, len))
	{
		seq += len;
	}

	mutex.Release();
	return 0;
}

bool TcpSession::HandlePacket(IPv4_Header *ip_hdr, TCP_Packet *tcp)
{
	TCP_Header *header = tcp->header;
	uint8 flags = tcp->header->flags;
	bool has_data = tcp->data_length > 0;

	if ((state == TCP_CLOSED || state == TCP_LISTEN) && flags != TCP_SYN)
		return false;

	switch (state)
	{
	case TCP_CLOSED:
		if (flags == TCP_SYN)
		{
			if (state == TCP_CLOSED)
			{
				seq = rand();
				ack = header->seq + 1;
				state = TCP_SYN_RECEIVED;
			}
		}
		break;

	case TCP_LISTEN:
		if (flags == TCP_SYN)
		{
			seq = rand();
			ack = header->seq + 1;

			debug_print("New session\n");

			sessions_mutex.Aquire();

			if (new_sockets.Count() >= backlog)
			{
				sessions_mutex.Release();
				return false;
			}

			sockaddr_in *addr = new sockaddr_in;
			addr->sin_family = AF_INET;
			addr->sin_port = htons(tcp->header->src_port);
			addr->sin_family = AF_INET;
			addr->sin_addr.s_addr = ip_hdr->src;

			Socket *new_socket = SocketManager::CreateSocket();
			new_socket->Init(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			new_socket->addr = (sockaddr *)addr;
			new_socket->src_port = tcp->header->dst_port;
			new_socket->tcp_session->HandlePacket(ip_hdr, tcp);
			new_sockets.Enqueue(new_socket);

			sessions_mutex.Release();
			session_event.Set();
		}
		break;

	case TCP_SYN_RECEIVED:
		if (flags & TCP_ACK)
		{
			seq += 1;

			state = TCP_ESTABLISHED;
			ack_event.Set();
			debug_print("Connected\n");
		}
		break;

	case TCP_SYN_SENT:
		if (flags & TCP_RST)
		{
			Send(TCP_ACK);
			Reset();
			ack_event.Set();
		}

		if (flags & TCP_SYN && flags & TCP_ACK)
		{
			Send(TCP_ACK);
			state = TCP_ESTABLISHED;
			ack_event.Set();
		}
		break;

	case TCP_ESTABLISHED:
		if (flags & TCP_FIN)
		{
			state = TCP_CLOSE_WAIT;
			Send(TCP_ACK);

			Reset();
			socket->HandleClose();
		}
		else if (has_data)
		{
			if (header->seq == ack)
			{
				ack += tcp->data_length;
				socket->HandleData(tcp->data, tcp->data_length);
			}
			else
			{
				debug_print("Missing packet\n");
			}

			Send(TCP_ACK);
		}

		if (flags & TCP_ACK)
			ack_event.Set();
		break;

	case TCP_FIN_WAIT_1:
		seq += 1;

		if (flags == TCP_FIN)
		{
			state = TCP_CLOSING;
		}
		else if (flags & TCP_ACK)
		{
			if (flags & TCP_FIN)
				state = TCP_TIME_WAIT;
			else
				state = TCP_FIN_WAIT_2;

			Send(TCP_ACK);
			ack_event.Set();
		}
		break;

	case TCP_FIN_WAIT_2:
		if (flags & TCP_FIN)
		{
			Send(TCP_ACK);
			state = TCP_TIME_WAIT;
			fin_event.Set();
		}
		break;

	case TCP_LAST_ACK:
		if (flags & TCP_ACK)
		{
			// TODO
			seq += 1;
			state = TCP_TIME_WAIT;
			Send(TCP_ACK); //
			debug_print("Passive close\n");
			Reset();
			socket->HandleClose();
		}
		break;

	case TCP_CLOSING:
		if (flags & TCP_ACK)
		{
			state = TCP_TIME_WAIT;
			ack_event.Set();
		}
		break;
	}

	return true;
}

bool TcpSession::SendWait(uint8 flags, int timeout)
{
	return SendWait(flags, 0, 0, timeout);
}

bool TcpSession::SendWait(uint8 flags, const void *buf, size_t len, int timeout)
{
	ack_event.Clear();

	if (!Send(flags, buf, len))
		return false;

	if (ack_event.Wait(timeout))
		return true;

	debug_print("Timed out\n");
	return false;
}

bool TcpSession::Send(uint8 flags)
{
	return Send(flags, 0, 0);
}

bool TcpSession::Send(uint8 flags, const void *buf, size_t len)
{
	NetPacket *pkt = TCP::CreatePacket((sockaddr_in *)socket->addr, socket->src_port, flags, seq, ack, buf, len);

	if (!pkt)
		return false;

	TCP::Send(pkt);
	return true;
}

void TcpSession::Reset()
{
	seq = 0;
	ack = 0;
	state = TCP_CLOSED;

	ack_event.Set();
	fin_event.Set();
	session_event.Set();

	ack_event.Clear();
	fin_event.Clear();
	session_event.Clear();
}
