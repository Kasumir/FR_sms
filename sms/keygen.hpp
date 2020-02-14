/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 **/
#ifndef __MSG_KEY_H__
#define __MSG_KEY_H__

#include <string>

using namespace std;

class keygen
{
	public:
		keygen();
		~keygen();

		static string create();
};

#endif /* __MSG_KEY_H__ */
