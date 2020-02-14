/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 **/
#include "cslog.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <string.h>

cslog cs_endl;

cslog * cslog::pinstance = 0;

cslog::cslog()
	: fout()
	, dir(".")
	, today_str()
	, f_err(false)
	, f_endl(false)
{
	today_str = current();
}

cslog::~cslog()
{
}


void cslog::open()
{
	std::cout << path();
	fout.open(path().c_str(), std::ios::app);
	if (!fout)
		f_err = true;
}

bool cslog::rotate()
{
	if (current() == today_str)
		return false;

	fout.close();
	fout.clear();

	settoday(current());

	open();

	return true;
}

std::string cslog::current() const
{
	struct tm tm_val;
	time_t t;
	char buf[9];

	memset(&buf, 0, sizeof(buf));
	memset(&tm_val, 0, sizeof(tm_val));

#ifdef _WIN32
	SYSTEMTIME st;

	GetLocalTime(&st);

	sprintf(buf, "%04u%02u%02u", st.wYear, st.wMonth, st.wDay);
#else
	t = time(NULL);
	(void)localtime_r(&t, &tm_val);

	strftime(buf, sizeof(buf), "%Y%m%d", &tm_val);
#endif

	return buf;
}

std::string cslog::getdate() const
{
	struct tm tm_val;
	time_t t;
	char buf[20];

	memset(&buf, 0, sizeof(buf));
	memset(&tm_val, 0, sizeof(tm_val));

#ifdef _WIN32
	SYSTEMTIME st;

	GetLocalTime(&st);

	sprintf(buf, "%04u-%02u-%02u %02u:%02u:%02u", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#else
	t = time(NULL);
	(void)localtime_r(&t, &tm_val);

	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_val);
#endif

	return buf;
}

void cslog::if_endl_print_date()
{
	if (f_endl)
	{
		fout << getdate() << "\t";
		if (!fout)
			f_err = true;
		f_endl = false;
	}
}

cslog & cslog::operator<<(const std::string & s)
{
	if (!pinstance) return *this;
	if (!fout.is_open()) return *this;
	if_endl_print_date();
	fout << s;
	if (!fout)
		f_err = true;

	return *this;
}

cslog & cslog::operator<<(const int i)
{
	if (!pinstance) return *this;
	if (!fout.is_open()) return *this;
	if_endl_print_date();
	fout << i;
	if (!fout)
		f_err = true;

	return *this;
}

cslog & cslog::operator<<(const uint32_t i)
{
	if (!pinstance) return *this;
	if (!fout.is_open()) return *this;
	if_endl_print_date();
	fout << i;
	if (!fout)
		f_err = true;

	return *this;
}

cslog & cslog::operator<<(cslog & _cs_endl)
{
	if (!pinstance) return *this;
	if (!fout.is_open()) return *this;
	if (&_cs_endl == &cs_endl)
	{
		fout << std::endl;
		if (!fout)
			f_err = true;
		f_endl = true;
//		fout << getdate() << "\t";
//		if (!fout)
//			f_err = true;
	}
	rotate();
	return *this;
}

#ifdef __EXAMPLE_TEST__
int main(void)
{
	cslog & log = *(cslog::instance());

	log.setdir("./log");
	log.open();


	int count=0;
	while(1) 
	{
		log << "TEST : " << "AAAAA" << count++ << cs_endl;
		if (!log)
		{
			std::cout << "error!" << endl;
			break;
		}
		sleep(1);
	}

	return 0;
}
#endif
