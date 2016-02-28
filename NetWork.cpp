#include "StdAfx.h"
#include "NetWork.h"

//添加库
#pragma comment(lib,"ws2_32.lib")

#define MAX_CONNECT 1

NetWork::NetWork(void)
{
	__fd = 0;
	__bak = 0;
}

int NetWork::socket_init()
{
	WORD	wVersionRequested;//版本号
	WSADATA wsaData;
	int		err;
	
	wVersionRequested = MAKEWORD( 2, 2 );//2.2版本的套接字
	//初始化套接字
	err = WSAStartup( wVersionRequested, &wsaData );
	
	if ( err != 0 )
	{
		return -1;
	}//加载套接字库，加裁失败则返回
	
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup();
		return -1; 
	}//如果不是2.2的则退出
	
	return 0;
}

int NetWork::Create(MyHost *host,int type)
{
	int ret = 0;
	int on;
	struct sockaddr_in addr ={0};

	unsigned long ul = 1;
	struct timeval timeout ;
	fd_set r;
	unsigned long ul1= 0 ;

	if( 0 != socket_init() )
		return -1;

	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	switch(type)
	{
	case TCP_SERVER:

		__bak = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);

		if( host->bind_local_port )
		{
			addr.sin_port = htons(host->localport);
		}

		ret = bind(__bak, (struct sockaddr *)&addr,sizeof(addr));

		break;
	case TCP_CLIENT:
		__fd = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);

		addr.sin_addr.S_un.S_addr = inet_addr(host->ip);
		addr.sin_family = AF_INET;
		addr.sin_port = htons(host->dstport);

		//绑定本机端口
		if( host->bind_local_port )
		{
			struct sockaddr_in tmp;
			tmp.sin_family = AF_INET;
			tmp.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			tmp.sin_port = htons(host->localport);
			bind(__fd,(struct sockaddr *)&tmp,sizeof(tmp));
		}
		/////////////////////////////////////////////////
		//设置非阻塞方式连接
		ret = ioctlsocket(__fd, FIONBIO, (unsigned long*)&ul);
		if(ret==SOCKET_ERROR)
			return -1;
		////////////////////////////////////////////////
		//连接到远端IP
		connect(__fd, (struct sockaddr *)&addr,sizeof(addr));
		
		//select 模型，即设置超时
		FD_ZERO(&r);
		FD_SET(__fd, &r);
		timeout.tv_sec = 5; //连接超时5秒
		timeout.tv_usec =0;
		ret = select(0, 0, &r, 0, &timeout);
		if ( ret <= 0 )
		{
			::closesocket(__fd);
			return -1;
		}
		//一般非锁定模式套接比较难控制，可以根据实际情况考虑 再设回阻塞模式
		ret = ioctlsocket(__fd, FIONBIO, (unsigned long*)&ul1);

		if(ret==SOCKET_ERROR)
		{
			::closesocket (__fd);
			return -1;
		}

		break;
	case UDP_SERVER:

		__fd = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);

		if( host->bind_local_port )
		{
			addr.sin_port = htons(host->localport);
		}

		ret = bind(__fd, (struct sockaddr *)&addr,sizeof(addr));

		break;
	case UDP_CLIENT:

		__fd = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP);

		__dst.sin_addr.S_un.S_addr = inet_addr(host->ip);
		__dst.sin_family = AF_INET;
		__dst.sin_port = htons(host->dstport);

		struct sockaddr_in tmp;

		//绑定本机端口
		if( host->bind_local_port )
		{
			tmp.sin_family = AF_INET;
			tmp.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			tmp.sin_port = htons(host->localport);
			ret = bind(__fd,(struct sockaddr *)&tmp,sizeof(tmp));
		}
		else
		{
			tmp.sin_family = AF_INET;
			tmp.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			//用户没有指定的话就我自己指定
			tmp.sin_port = htons(57132);

			ret = bind(__fd,(struct sockaddr *)&tmp,sizeof(tmp));
		}
	
		break;
	default:
		break;
	}

	__mode = type;

	return ret;

}

int NetWork::Read(void *buffer,int size)
{
	int len = sizeof(__dst);
	switch(__mode)
	{
	case TCP_SERVER:
	case TCP_CLIENT:
		return recv(__fd,(char *)buffer,size,0);
	case UDP_SERVER:
	case UDP_CLIENT:
		return recvfrom(__fd,(char *)buffer,size,0,(struct sockaddr *)&__dst,&len);
	}

	return 0;
}
int NetWork::Write(void *buffer,int size)
{
	int len = sizeof(__dst);
	switch(__mode)
	{
	case TCP_SERVER:
	case TCP_CLIENT:
		return send(__fd,(char *)buffer,size,0);
	case UDP_SERVER:
	case UDP_CLIENT:
		return sendto(__fd,(char *)buffer,size,0,(struct sockaddr *)&__dst,len);
	}

	return 0;
}

int NetWork::GetAccept(void)
{
	struct sockaddr_in addr ={0};
	int socklen = sizeof(addr);
	//最大连接数10
	listen(__bak,MAX_CONNECT);

	__fd = accept(__bak,(struct sockaddr *)&addr,&socklen);

	return __fd;
}

int NetWork::GetDstInfo(struct sockaddr_in *addr)
{
	return 0;
}

int NetWork::Close()
{
	closesocket(__fd);

	if( (__mode == TCP_SERVER) || (__mode == UDP_SERVER) )
	{
		closesocket(__bak);
	}

	::WSACleanup();//必须调用这个函数清除参数
	return 0;
}

NetWork::~NetWork(void)
{
}



