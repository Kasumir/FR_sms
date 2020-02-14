/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/
#include "token.hpp"

std::string trim(std::string & s, const std::string & drop)
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}


void tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters)
{
	std::string::size_type begin = str.find_first_not_of(delimiters, 0);
	std::string::size_type end = str.find_first_of(delimiters, begin);

	while (end != std::string::npos || begin != std::string::npos)
	{
		tokens.push_back(str.substr(begin, end - begin));
		begin = str.find_first_not_of(delimiters, end);
		end = str.find_first_of(delimiters, begin);
	}
}

void tokenize_once(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters)
{
	std::string::size_type boundary = str.find_first_of(delimiters, 0);
	tokens.push_back(str.substr(0, boundary));
	tokens.push_back(str.substr(boundary+1));
}

