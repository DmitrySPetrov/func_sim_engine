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

#ifndef __LIBKMS_BLOCK_CLASS_INET_ADDRESS_HPP__
#define __LIBKMS_BLOCK_CLASS_INET_ADDRESS_HPP__

#include <string>

namespace LIBKMS_namespace{ namespace std_blcl {

struct InetAddress {
	std::string ip;
	uint16_t port;

	InetAddress(): port( 0 ) {}
	InetAddress( uint16_t p ): port( p ) {}
};

}}	//	namespace LIBKMS_namespace::std_blcl

#endif
