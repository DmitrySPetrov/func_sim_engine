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

#include "block_class/udp_networking.hpp"

#include <iostream>


namespace LIBKMS_namespace{ namespace std_blcl { namespace udp_exchange {

namespace ba = boost::asio;


template< typename P >
udp_server::udp_server
		( ba::io_service & io, ba::ip::udp::endpoint ep, const P & f ):
	_socket( io, ep ),
	_add( f )
{
	do_receive();
}

void udp_server::do_receive() {
	_socket.async_receive_from( ba::buffer( _buf, max_data_length ),
		_remote_ep,
		[this]( const boost::system::error_code & er, std::size_t len ) {
			if( !er && len > 0 ) {
				_add( msg_buffer( _buf, _buf+len ) );
			}
			do_receive();
		} );
}


udp_server_caller::udp_server_caller( const std::string & addr, uint32_t port ):
	_srv( _io,
		ba::ip::udp::endpoint( ba::ip::address::from_string( addr ), port ),
		[this]( const msg_buffer & b ){ return add_message( b ); } ),
	_run_thread( [this]() {
			try {
				_io.run();
			} catch( std::exception & e ) {
				std::cerr << "Ошибка: " << e.what() << std::endl;
			}
		} )
{ }

udp_server_caller::~udp_server_caller() {
	_io.stop();
	_run_thread.join();
}

bool udp_server_caller::add_message( const msg_buffer & msg ) {
	if( msg.size() > 0 ) {
		_mtx.lock();
		_msgs.push_back( msg );
		_mtx.unlock();
	}
	return true;
}

std::list< msg_buffer > udp_server_caller::get_messages() {
	std::list< msg_buffer > res;

	_mtx.lock();
	res.splice( res.end(), _msgs );
	_mtx.unlock();

	return res;
}


udp_client::udp_client( const std::string & addr, uint32_t port ):
	_ep( ba::ip::udp::endpoint( ba::ip::address::from_string( addr ), port ) ),
	_socket( _io, ba::ip::udp::endpoint( ba::ip::udp::v4(), 0 ) )
{}

udp_client::~udp_client() {
	_io.stop();
	_run_thread.join();
}

void udp_client::send_message( const msg_buffer & buf ) {

	//	Только посылаем, ответа не ждем
	_run_thread = std::move( boost::thread(
		[this]( const msg_buffer & buf ) {
			try {			
				_socket.send_to( ba::buffer( buf ), _ep );
			} catch( boost::system::system_error & e ) {
				std::cerr << "Ошибка при посылке пакета: " << e.what()
					<< std::endl;
			}
		}, buf ) );
}


}}}	//	namespace LIBKMS_namespace::std_blcl::udp_exchange

