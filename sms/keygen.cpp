/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 **/

#include "keygen.hpp"
#ifdef _WIN32
#include <time.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <string.h>

static int time_format(const char *format, time_t t, size_t bufsiz, char *buf)
{
	struct tm _tm;
#ifdef _WIN32
	(void)_localtime64_s (&_tm, &t);
#else
	(void)localtime_r (&t, &_tm);
#endif

	return ( (strftime (buf, bufsiz, format, &_tm)) > 0 ? 1 : 0 );
}

string keygen::create()
{
	char date[20];
	char usec[20];
	char randval[20];
	char key[31];

	memset(date, 0x00, sizeof(date));
	memset(usec, 0x00, sizeof(usec));
	memset(randval, 0x00, sizeof(randval));
	memset(key, 0x00, sizeof(key));

	::time_format("%Y%m%d%H%M%S", time(NULL), 20, date);

	struct timeval tv;
#ifdef _WIN32
	SYSTEMTIME st;
	GetSystemTime(&st);
	tv.tv_sec = st.wSecond;
	tv.tv_usec = st.wMilliseconds;
#else
	struct timezone tz;
	gettimeofday(&tv, &tz);
	tv.tv_sec = tv.tv_sec;
	tv.tv_usec = tv.tv_sec;
#endif

	sprintf(usec, "%06ld", tv.tv_usec);
	sprintf(randval, "%06.f", 999999.0 * rand() / (RAND_MAX + 1.0));
	sprintf(key, "%s%s%s", date, usec, randval);

	return key;
}
