/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 **/
#ifndef __cslog_h__
#define __cslog_h__

#define LOG	log << __FILE__ << ":" << __LINE__ << ":"

#include <fstream>
#include <string>
#ifdef _WIN32
#include "stdint.h"
#else
#include <stdint.h>
#endif

class cslog
{
	public:
		cslog();
		~cslog();

	public:
		static cslog * instance() { if (!pinstance) pinstance = new cslog; return pinstance; }
		void setdir(const std::string & _dir) { this->dir = _dir; }
		void open();
		void close() { fout.close(); }

		//cslog & write();
		//cslog & printf();
		bool good() const { return f_err == false; }
		bool fail() const { return f_err; }

		operator void*() const { return fail() ? (void*)0 : (void*)(-1); }
		bool operator!() const { return fail(); }
		cslog & operator<<(const int i);
		cslog & operator<<(const uint32_t i);
		cslog & operator<<(const char ch);
		cslog & operator<<(const std::string & s);
		cslog & operator<<(cslog & cs_endl);

	private:
		bool rotate();
		std::string current() const;
		std::string getdate() const;
		std::string path() const { return dir + "/" + today_str; }
		void settoday(const std::string & date) { today_str = date; }
		const std::string & today() const { return today_str; }
		void if_endl_print_date();
	private:
		std::ofstream fout;
		std::string dir;
		std::string today_str;
		bool f_err;
		bool f_endl;
		static cslog * pinstance;
};

extern cslog cs_endl;

#endif /* __cslog_h__ */
