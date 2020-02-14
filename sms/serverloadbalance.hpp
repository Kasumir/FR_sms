/**
 * vi:set ts=4 sw=4 noexpandtab enc=cp949:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/

#ifndef __serverloadbalance__
#define __serverloadbalance__

#include "tcpconn.hpp"
#include <list>

namespace coolsms {
	class slb {
		public:
			static bool connect(tcpconn & tcp, bool is_resale);

		private:
			static int host_numbers;
			static char *host_array[3];
			static uint16_t port[3];
	};	
}

#endif /* __serverloadbalance__ */
