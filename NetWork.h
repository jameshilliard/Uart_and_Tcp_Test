#pragma once

#define TCP_SERVER  1
#define TCP_CLIENT	2
#define UDP_SERVER  3
#define	UDP_CLIENT  4

struct _MyHost
{
	char ip[16];
	unsigned short dstport;
	int		bind_local_port;
	unsigned short localport;
};

typedef _MyHost MyHost;

class NetWork
{
public:
	NetWork(void);
	int Create(MyHost *host,int type);

	int Read(void *buffer,int size);
	int Write(void *buffer,int size);

	int GetDstInfo(struct sockaddr_in *addr);

	int Close();
	~NetWork(void);
private:
	SOCKET __fd;
	SOCKET __bak;
	int	   __mode;
	struct sockaddr_in __dst;
	int	socket_init();
public:
	int GetAccept(void);
};

