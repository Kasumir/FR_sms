/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/
#ifndef __tcpconn_h__
#define __tcpconn_h__

#include <sstream>
#ifdef _WIN32
#include <winsock2.h>
#include "stdint.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <unistd.h>
#endif
#include <string>

#ifdef _DEBUG
#include "cslog.hpp"
#endif

#define BUFLEN	1024

#ifdef _WIN32
extern int winsock_startup(void);
extern void winsock_cleanup(void);
#endif

class tcpconn
{
	public:
		tcpconn();
		tcpconn(int sockfd);
		~tcpconn();

		bool init();
		bool getaddr32(const std::string & saddr, uint32_t & addr);
		void setnonblock();
		bool connect(const std::string & ipaddr, const uint16_t port);
		void close() { 
#ifdef _WIN32
			::closesocket(sockfd);
#else
			::close(sockfd); 
#endif
			sockfd = -1;
			connected = false; 
		}
		int send(const std::string & packet) { return send(packet.c_str(), packet.length()); }
		int send(const void *data, const int len);
		int recv(const int len, char *buf);
		int recvfull(const int len, char *buf);
		int getline(std::string & line);
		bool listen(const std::string & ipaddr, int port);
		int accept();
		int accept(std::string & remote_ipaddr);
		int getsock() const { return sockfd; }
		bool is_connected() const { return connected; }
		bool is_disconnected() const { return !connected; }

	private:
		int sockfd;
		bool connected;
		std::stringstream rdbuf;
#ifdef _DEBUG
		cslog & log;
#endif

};

#endif /* __tcpconn_h__ */
