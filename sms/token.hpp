/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/
#ifndef __token_h__
#define __token_h__

#include <string>
#include <vector>

extern std::string trim(std::string & s, const std::string & drop = " ");
extern void tokenize(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters = " ");
extern void tokenize_once(const std::string & str, std::vector<std::string> & tokens, const std::string & delimiters = " ");

#endif /* __token_h__ */
