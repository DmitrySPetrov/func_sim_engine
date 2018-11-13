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
