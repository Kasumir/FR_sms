/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 **/
#ifndef __CSCP_H__
#define __CSCP_H__

#include <vector>
#include <stdlib.h>
#include <string.h>
#include "tcpconn.hpp"

#define CSCP_VERSION		"CSCP2.0"
#define CSCP_VERSION_SIZE	7
#define CSCP_BODYLEN_SIZE	8

#define CSCPPARAM_ID_SIZE		2
#define CSCPPARAM_BODYLEN_SIZE	8
#define CSCPPARAM_ID_MESSAGE	"ME"
#define CSCPPARAM_ID_ATTACHMENT	"AT"


class CSCPPARAM
{
	public:
		CSCPPARAM() : id(), body(0), packet(0), bodylen(0) {}
		~CSCPPARAM() { if (body) free(body); if (packet) free(packet); }
		void setId(std::string _id) { this->id = _id;}
		void setBody(void * _body, int _len) { this->body = _body; this->bodylen = _len; }
		void setBody(std::string _body) { setBody(strdup(_body.c_str()), _body.size()); }
		void *getPacketBytes() { return packet; }
		std::string getPacketString() { return std::string((char *)packet); }
		bool read(tcpconn & tcp);
		int size() { return CSCPPARAM_ID_SIZE + CSCPPARAM_BODYLEN_SIZE + bodylen; }
		std::string getBody() { return std::string((char*)this->body); }
		std::string getId() { return this->id; }
		void build();

	public:
		std::string id;
		void *body;
		void *packet;
		int bodylen;
};

class CSCP
{
	public:
		CSCP() : bodylen(0), params() { }
		~CSCP() { clear(); }
		bool read(tcpconn & conn);
		void build(std::string & cscpstr);
		void build(void **cscpdata, long *len);
		void add(CSCPPARAM * param);
		void addparam(std::string id, std::string body);
		void addparam(std::string id, void *body, int len);
		CSCPPARAM * getparam(std::string id);
		void clear();

	public:
		int bodylen;
		std::vector<CSCPPARAM *>params;
};

#endif /* __CSCP_H__ */
