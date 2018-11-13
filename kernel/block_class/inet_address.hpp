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
