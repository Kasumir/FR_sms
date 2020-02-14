/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 **/
#include <stdio.h>
#include "cslog.hpp"
#include "CSCP.hpp"

bool CSCP::read(tcpconn & conn)
{
	char buf[1024];
	cslog & log = *(cslog::instance());

	memset(buf, 0, sizeof(buf));
	if (conn.recvfull(CSCP_VERSION_SIZE, buf) <= 0) return false;
	memset(buf, 0, sizeof(buf));
	if (conn.recvfull(CSCP_BODYLEN_SIZE, buf) <= 0) return false;

	this->bodylen = atoi(buf);
	log << "CSCP:bodylen = " << this->bodylen << cs_endl;

	if (this->bodylen > 0)
	{
		int rest = this->bodylen;
		int rs=0;
		do {
			CSCPPARAM * param = new CSCPPARAM();
			param->read(conn);
			rest -= param->size();
			add(param);
/*
			if (rest >= (int)sizeof(buf)-1)
				rcvsz = sizeof(buf)-1;
			else
				rcvsz = rest;
			rs = conn.recvfull(rcvsz, buf);
			if (rs <= 0) 
			{
				LOG << "conn.recv error" << cs_endl;
				return false;
			} else {
				buf[rs] = 0x00;
				contents += buf;
			}
*/
			rest -= rs;
		} while(rest > 0); 
	}

	return true;
}

void CSCP::add(CSCPPARAM * param)
{
	params.push_back(param);
	this->bodylen += param->size();
}

void CSCP::addparam(std::string id, std::string body)
{
	CSCPPARAM * param = new CSCPPARAM();
	param->setId(id);
	param->setBody(body);
	param->build();
	add(param);
}

void CSCP::addparam(std::string id, void *body, int len)
{
	CSCPPARAM * param = new CSCPPARAM();
	param->setId(id);
	param->setBody(body, len);
	param->build();
	add(param);
}

CSCPPARAM * CSCP::getparam(std::string id)
{
	std::vector<CSCPPARAM *>::iterator it;

	for (it = params.begin(); it != params.end(); it++) {
		if ((*it)->getId() == id) {
			return *it;
		}
	}
	return 0;
}

void CSCP::build(std::string & cscpstr)
{
	char buf[80];

	sprintf(buf, "%08d", this->bodylen);
	cscpstr = CSCP_VERSION;
    cscpstr += buf;
	std::vector<CSCPPARAM *>::iterator it;

	for (it = params.begin(); it != params.end(); it++) {
		cscpstr += (*it)->getPacketString();
	}
}

void CSCP::build(void **cscpdata, long *len)
{
	char buf[80];

	*len = CSCP_VERSION_SIZE + CSCP_BODYLEN_SIZE + this->bodylen;
	*cscpdata = malloc(*len);

	char *p = (char *)*cscpdata;

	sprintf(buf, "%08d", this->bodylen);

	memcpy(p, CSCP_VERSION, CSCP_VERSION_SIZE);
	p += CSCP_VERSION_SIZE;

	memcpy(p, buf, CSCP_BODYLEN_SIZE);
	p += CSCP_BODYLEN_SIZE;

	std::vector<CSCPPARAM *>::iterator it;
	for (it = params.begin(); it != params.end(); it++) {
		memcpy(p, (*it)->getPacketBytes(), (*it)->size());
		p += (*it)->size();
	}
}

void CSCP::clear()
{
	std::vector<CSCPPARAM *>::iterator it;
	for (it = params.begin(); it != params.end(); it++) {
		delete *it;
	}
	params.clear();
}


void CSCPPARAM::build()
{
	packet = malloc(CSCPPARAM_ID_SIZE + CSCPPARAM_BODYLEN_SIZE + this->bodylen);

	char *p = (char *)packet;

	memcpy(p, (void *)this->id.c_str(), CSCPPARAM_ID_SIZE);
	p += CSCPPARAM_ID_SIZE;

	char buf[80];
	sprintf(buf, "%08d", this->bodylen);
	memcpy(p, (void *)buf, CSCPPARAM_BODYLEN_SIZE);
	p += CSCPPARAM_BODYLEN_SIZE;

	memcpy(p, this->body, bodylen);
	p += bodylen;
}

bool CSCPPARAM::read(tcpconn & conn)
{
	char buf[80];
	std::string tmp_bodylen;

	memset(buf, 0, sizeof(buf));
	if (conn.recvfull(CSCPPARAM_ID_SIZE, buf) <= 0) return false;
	this->id = buf;
	memset(buf, 0, sizeof(buf));
	if (conn.recvfull(CSCPPARAM_BODYLEN_SIZE, buf) <= 0) return false;
	tmp_bodylen = buf;

	this->bodylen = atoi(tmp_bodylen.c_str());
	if (this->bodylen > 0) {
		body = malloc(this->bodylen);
		if (!body) return false;
		if (conn.recvfull(this->bodylen, (char *)body) <= 0) return false;
	}
	return true;
}
