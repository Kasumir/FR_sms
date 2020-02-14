/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/
#ifndef __TBSP_H__
#define __TBSP_H__

#include <string>
#include <map>
#include <vector>

class TBSP
{
	public:
		static void build(std::map<std::string,std::string> & tbspmap, std::string & tbspstr);
		bool parse(const std::string & tbspstr);
		bool getval(const std::string & key, std::string & val);
		bool getval(const std::string & key, long & val);
		bool getbool(const std::string & key, bool & val);
	private:
		std::map<std::string,std::string> tbspmap;
};


#endif /* __TBSP_H__ */
