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
