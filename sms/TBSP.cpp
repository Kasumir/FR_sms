/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/
#include <stdlib.h>
#include "TBSP.hpp"
#include "token.hpp"

static void getmultiline(const std::string & line, const std::string & sep, std::string & multiline)
{
	for (size_t i = 0; i < line.length(); i++)
	{
		if (line.at(i) == '\n')
		{
			multiline += sep;
		} else {
			multiline += line.at(i);
		}
	}
}


static std::string tolowercase(const std::string & str)
{
	std::string rstr;

	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (isupper(str.at(i)))
			rstr += tolower(str.at(i));
	}

	return rstr;
}

/*
void tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters = " ")
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

void tokenize_once(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters = " ")
{
	std::string::size_type boundary = str.find_first_of(delimiters, 0);
	tokens.push_back(str.substr(0, boundary));
	tokens.push_back(str.substr(boundary+1));
}

*/

void TBSP::build(std::map<std::string,std::string> & tbspmap, std::string & tbspstr)
{
	std::map<std::string,std::string>::iterator it;

	for (it = tbspmap.begin(); it != tbspmap.end(); it++)
	{
		if (it->first == "MESSAGE")
		{
			std::string multiline;
			getmultiline(it->second, "\nMESSAGE:", multiline);
			tbspstr += it->first;
			tbspstr += ":";
			tbspstr += multiline;
		} else {
			tbspstr += it->first;
			tbspstr += ":";
		       	tbspstr += it->second;
		}
		tbspstr += "\n";
	}
}

bool TBSP::parse(const std::string & tbspstr)
{
	std::vector<std::string> tokens;
	std::string key, value;

	tokenize(tbspstr, tokens, "\n");

	std::vector<std::string>::iterator it;

	for (it = tokens.begin(); it != tokens.end(); it++)
	{
		//std::cout << *(it) << std::endl;

		key = "";
		value = "";
		std::vector<std::string> subtokens;
		std::vector<std::string>::iterator subit;
		tokenize_once(*(it), subtokens, ":");
		subit = subtokens.begin();
		if (subit != subtokens.end())
		{
			//std::cout << "key: " << *(subit) << endl;
			key = *(subit);
			subit++;
		}
		if (subit != subtokens.end())
		{
			value = *(subit);
			subit++;
		}
		if (key == "MESSAGE")
		{
			if (tbspmap.find(key) == tbspmap.end())
				tbspmap[key] += value;
			else
				tbspmap[key] += "\n" + value;
		} else
			tbspmap[key] = value;
	}

	return true;
}

bool TBSP::getval(const std::string & key, std::string & val)
{
	std::map<std::string,std::string>::iterator it = tbspmap.find(key);
	if (it != tbspmap.end())
	{
		//std::cout << "found key:" << key << endl;
		val = tbspmap[key];
		//std::cout << "found val:" << val << endl;
		return true;
	}

	return false;
}

bool TBSP::getval(const std::string & key, long & val)
{
	std::string _val;
	bool ret = getval(key, _val);
	val = atol(_val.c_str());
	return ret;
}

bool TBSP::getbool(const std::string & key, bool & val)
{
	std::string meta;


	if (getval(key, meta))
	{
		meta = tolowercase(meta);

		if (meta == "true")
			return true;
	}
	return false;
}
