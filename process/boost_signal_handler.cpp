#include "boost_signal_handler.hpp"

#include <iostream>
#include <thread>

namespace kms { namespace process {

BoostSignalHandler::BoostSignalHandler( signal_handler_t h ):
		_io(),
		_sig( _io, SIGINT, SIGTERM ),
		_h( h )
{
	async_wait();
	std::thread( [ this ](){ _io.run(); } ).detach();
}

void BoostSignalHandler::async_wait() {
	_sig.async_wait( [this]( const boost::system::error_code & ec, int snum ) {
				if( !ec ) {
					async_wait();
					_h( snum );
				}
			} );
}

}}	//	namespace kms::process
