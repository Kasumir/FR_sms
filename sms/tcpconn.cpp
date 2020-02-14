/**
 * vi:set ts=4 sw=4 expandtab enc=utf-8:
 * Copyright (C) 2008-2010 D&SOFT
 * Choi Dae-Sung <diver@coolsms.co.kr>
 **/
#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#ifndef _WIN32
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#endif
#include "tcpconn.hpp"


#ifdef _WIN32
int winsock_startup(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );

	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return -1;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 ) {
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			::WSACleanup( );
			return -1;
	}

	return 0;
}

void winsock_cleanup(void)
{
	::WSACleanup( );
}
#endif

tcpconn::tcpconn()
	: sockfd(-1)
	, connected(false)
	, rdbuf()
#ifdef _DEBUG
	, log(*(cslog::instance()))
#endif
{
}

tcpconn::tcpconn(int _sockfd)
	: sockfd(_sockfd)
#ifdef _DEBUG
	, log(*(cslog::instance()))
#endif
{
}


tcpconn::~tcpconn()
{
	close();
}

bool tcpconn::init()
{
	int one=1;
	int res;
	struct timeval tv = {3, 0};
#ifdef _WIN32
	unsigned long timeout = 3000;
#endif

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		return false;
	}
#ifdef _DEBUG
	log << "sockfd: " << sockfd << cs_endl;
#endif

        res = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one));
	if(res < 0) {
		fprintf(stderr, "setsockopt error\n");
		return false;
	}

#ifdef _WIN32
	res = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(tv));
#else
	res = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const void *)&tv, sizeof(tv));
#endif
       	if(res < 0) {
	       	fprintf(stderr, "setsockopt error\n");
		return false;
       	} 

#ifdef _WIN32
	res = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(tv));
#else
	res = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const void *)&tv, sizeof(tv));
#endif
       	if(res < 0) {
	       	fprintf(stderr, "setsockopt error\n");
		return false;
       	}
	
	return true;
}

bool tcpconn::getaddr32(const std::string & saddr, uint32_t & addr)
{
	int i;
	struct hostent *host;
#ifdef INET_ATON
	struct in_addr iaddr;

	/* interprets the character string which represents a numeric internet address */
	if(inet_aton(saddr.c_str(), &iaddr) != 0)
		addr = iaddr.s_addr ;
	else
#else
	addr = inet_addr(saddr.c_str());
#endif
        if(addr == (uint32_t)-1)
	{	/*
		 * interprets domain name address if not numeric address.
		 */

		/* Old version doesn't support for interpreting the character string numeric address like "10.200.32.218" */
		host = gethostbyname(saddr.c_str());
		if(!host) return false;   /* none host */

		for(i = 0; i < 4; i++) {  /* 4바이트 주소를 (uint)addr에 넣기 */
			addr = addr << 8;
			addr += (unsigned char)host->h_addr[i];
		}

		addr = htonl(addr) ;
	} /* else */

	return true;
}

/*
void tcpconn::setnonblock()
{
	int val=0;

	val = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
}
*/

bool tcpconn::connect(const std::string & ipaddr, const uint16_t port)
{
	int res = 0;
	int dstaddrlen;
	struct sockaddr_in dstaddr;

	memset(&dstaddr, 0, sizeof(dstaddr));
	// dstaddr.sin_addr.s_addr = getNetAddress(ipaddr.c_str());
	if (!getaddr32(ipaddr, (uint32_t &)dstaddr.sin_addr.s_addr))
	{
#ifdef _DEBUG
		log << "getaddr32 error" << cs_endl;
#endif
		return false;
	}
	dstaddr.sin_family = AF_INET;
	dstaddr.sin_port = htons(port);
	dstaddrlen = sizeof(dstaddr);

	res = ::connect(sockfd, (struct sockaddr *)&dstaddr, dstaddrlen);
	if (res == -1)
	{
		close();
		return false;
	}

	connected = true;

	return true;
}

int tcpconn::send(const void *data, const int len)
{
	int res=0;
	int nSent=0;
	int countDown=0;

	countDown = len;

	while (1) 
	{
		res = ::send(sockfd, (char *)data + nSent, countDown, 0);
                if(res == 0) 
		{
#ifdef _DEBUG
                        LOG << "connection reset or timeout" << cs_endl;
#endif
                        return 0;
                }
                if(res < 0) 
		{
#ifdef _DEBUG
                        LOG << "send error" << cs_endl;
#endif
                        return -1;
                }

                countDown -= res;
                nSent += res;
                if(countDown <= 0) break;
        }

	if(countDown != 0) return -1;

	return len;
}

/**
 * 0 : timeout, -1 : error
 */
int tcpconn::recv(const int len, char *buf)
{
	int ret;

	ret = ::recv(sockfd, buf, len, 0);
	return ret;
}

/**
 * -1 : end of stream, 0 : empty line
 */
int tcpconn::getline(std::string & line)
{
	char buf[BUFLEN+1];
	char str[BUFLEN+1];
	std::string tmp;
	bool bfound=false;

	memset(buf, 0, sizeof(buf));
	memset(str, 0, sizeof(buf));

	while (1)
	{
		std::stringbuf * sb = rdbuf.rdbuf();
		if (sb->in_avail() > 0)
		{
			if(rdbuf.getline(str, BUFLEN))
			{
				tmp = str;
				bfound = true;
				break;
			}
			else
			{
				bfound = false;
			}
		}
		if (bfound == false)
		{
			int ret = ::recv(sockfd, buf, BUFLEN, 0);
			if (ret <= 0)
			{
				return -1;
			}
			buf[ret] = 0;

			rdbuf << buf << std::endl;
		}
	}

	line = tmp;

	return tmp.size();
}

int tcpconn::recvfull(const int len, char *buf)
{
        int res=0;
        int nRcvd=0;
        int countDown=0;

        countDown = len;

        char *p = buf;

        while(1) {
                res = ::recv(sockfd, p, countDown, 0);
                if(res == 0) {
                        fprintf(stderr, "recv time out\n");
                        return -1;
                }
                if(res < 0) {
                        fprintf(stderr, "recv error\n");
                        return -1;
                }
                p += res;

                countDown -= res;
                nRcvd += res;
                if(countDown <= 0) break;
        }
        if(countDown != 0) return -1;

        return len;
}

bool tcpconn::listen(const std::string & ipaddr, int port)
{
	int res;
	int reuseflag = 1;
	int srcaddrlen=0;
	struct sockaddr_in srcaddr;

	memset(&srcaddr, 0, sizeof(srcaddr));
	if (!getaddr32(ipaddr, (uint32_t &)srcaddr.sin_addr.s_addr))
	{
#ifdef _DEBUG
		log << "getaddr32 error" << cs_endl;
#endif
		return false;
	}
	srcaddr.sin_port = htons(port);
	srcaddrlen = sizeof(srcaddr);

#ifdef _WIN32
	res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseflag, sizeof(reuseflag));
#else
	res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&reuseflag, sizeof(reuseflag));
#endif
	if(res < 0) {
#ifdef _DEBUG
		log << "setsockopt error" << cs_endl;
#endif
		return false;
	}

	res = bind(sockfd, (struct sockaddr *)&srcaddr, srcaddrlen);
	if(res < 0) 
		return false;

	if (::listen(sockfd, 10) == -1) 
		return false;

	return true;
}

int tcpconn::accept()
{
	int newfd = 0;
	struct sockaddr_in cliaddr;
#ifdef _WIN32
	int cliaddrlen = 0;
#else
	socklen_t cliaddrlen = 0;
#endif

	cliaddrlen = sizeof(cliaddr);
	newfd = ::accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddrlen);

#ifdef _DEBUG
	log << "Accept IP ADDR : " << inet_ntoa(cliaddr.sin_addr) << cs_endl;
#endif

	return newfd;
}

int tcpconn::accept(std::string & remote_ipaddr)
{
	int newfd = 0;
	struct sockaddr_in cliaddr;
#ifdef _WIN32
	int cliaddrlen = 0;
#else
	socklen_t cliaddrlen = 0;
#endif

	cliaddrlen = sizeof(cliaddr);
	newfd = ::accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddrlen);

	remote_ipaddr = inet_ntoa(cliaddr.sin_addr);

#ifdef _DEBUG
	log << "Accept IP ADDR : " << remote_ipaddr << cs_endl;
#endif

	return newfd;
}
