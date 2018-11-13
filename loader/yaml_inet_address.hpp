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

#ifndef __LOADER_YAML_INET_ADDRESS_HPP__
#define __LOADER_YAML_INET_ADDRESS_HPP__

#include <block_class/inet_address.hpp>

#include <yaml-cpp/yaml.h>

#include <yaml_detail.hpp>

namespace kms_kernel { namespace std_blcl {

namespace udp_exchange {

const std::string ADDRESS_IP = "ip";
const std::string ADDRESS_PORT = "порт";

}	//	namespace udp_exchange

} }	//	namespace kms_kernel::std_blcl

namespace YAML {

template<>
struct convert< kms_kernel::std_blcl::InetAddress > {

static bool decode( const Node & node, kms_kernel::std_blcl::InetAddress & addr ) {

	using namespace kms_kernel::std_blcl::udp_exchange;

	if( !validate_map( node ) )
		{ return false; }

	if( !read_n_validate( node[ ADDRESS_IP ], "IP-адрес", NodeType::Scalar,
			addr.ip ) )
		{ return false; }

	if( !read_n_validate( node[ ADDRESS_PORT ], "порт", NodeType::Scalar,
			addr.port ) )
		{ return false; }

	return true;
}

};

}	//	namespace YAML

#endif
