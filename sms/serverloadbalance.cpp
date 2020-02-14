/**
 * vi:set ts=4 sw=4 expandtab fileencoding=utf8:
 * Copyright (C) 2008-2010 D&SOFT
 * http://open.coolsms.co.kr
 **/
#include "serverloadbalance.hpp"
#include <iostream>

namespace coolsms {
	int slb::host_numbers = 3;
	char *slb::host_array[] = {"alpha.coolsms.co.kr", "bravo.coolsms.co.kr", "delta.coolsms.co.kr"};
	uint16_t slb::port[] = {80, 80, 80};

	bool slb::connect(tcpconn & tcp, bool is_resale) {
		bool is_connected = false;
		
        for (int i = 0; i < host_numbers; i++) {
            is_connected = tcp.connect(host_array[i], port[i]);
            if (is_connected) break;
        }

		return is_connected;
	}
}
