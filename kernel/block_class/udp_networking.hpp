////////////////////////////////////////////////////////////////////////////////
// 
//  This file is part of Functional Simulation Engine.
//
//  Functional Simulation Engine is free software: you can redistribute it
//  and/or modify it under the terms of the GNU General Public License as
//  published by the Free Software Foundation, either version 3 of the License,
//  or (at your option) any later version.
//
//  Functional Simulation Engine is distributed in the hope that it will be
//  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
//  Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with Functional Simulation Engine.
//  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __LOADER_BLOCK_CLASS_UDP_NETWORKING_HPP__
#define __LOADER_BLOCK_CLASS_UDP_NETWORKING_HPP__

#include <detail/namespace.hpp> 

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include <vector>
#include <list>
#include <string>


namespace LIBKMS_namespace{ namespace std_blcl {

namespace udp_exchange {

typedef std::vector< uint8_t > msg_buffer;

typedef std::function< bool( const msg_buffer & ) > add_msg_func_t;

class udp_server {
public:
	template< typename P >
	udp_server( boost::asio::io_service &, boost::asio::ip::udp::endpoint, const P & );

private:
	void do_receive();

private:
	boost::asio::ip::udp::socket _socket;
	boost::asio::ip::udp::endpoint _remote_ep;

	enum{ max_data_length=1024 };
	uint8_t _buf[ max_data_length ];

	add_msg_func_t _add;
};	//	class udp_server


class udp_server_caller {
public:
	udp_server_caller( const std::string &, uint32_t );
	~udp_server_caller();

	bool add_message( const msg_buffer & );

	std::list< msg_buffer > get_messages();

private:
	boost::asio::io_service _io;
	udp_server _srv;

	std::list< msg_buffer > _msgs;

	boost::mutex _mtx;
	boost::thread _run_thread;
};	//	class udp_server_caller


class udp_client {
public:
	udp_client( const std::string &, uint32_t );
	~udp_client();

	void send_message( const msg_buffer & );

private:
	boost::asio::io_service _io;
	boost::asio::ip::udp::endpoint _ep;
	boost::asio::ip::udp::socket _socket;

	boost::thread _run_thread;
};


}	//	namespace udp_exchange

}}	//	namespace LIBKMS_namespace::std_blcl

#endif
